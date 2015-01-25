/*
  connectionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "connectionmodel.h"
#include "probe.h"
#include "readorwritelocker.h"

#include <common/metatypedeclarations.h>
#include "util.h"

#include <QColor>
#include <QDebug>
#include <QMetaMethod>
#include <QMetaObject>
#include <QThread>

using namespace GammaRay;

static bool checkMethodForObject(QObject *obj, const QByteArray &signature, bool isSender)
{
  if (!obj || signature.isEmpty()) {
    return false;
  }
  const QMetaObject *mo = obj->metaObject();
  const int methodIndex = mo->indexOfMethod(signature.constData() + 1);
  if (methodIndex < 0) {
    return false;
  }
  const QMetaMethod method = mo->method(methodIndex);
  if (method.methodType() != QMetaMethod::Signal &&
      (isSender || method.methodType() != QMetaMethod::Slot)) {
    return false;
  }
  const int methodCode = signature.at(0)  - '0';
  if ((methodCode == QSLOT_CODE && method.methodType() != QMetaMethod::Slot) ||
      (methodCode == QSIGNAL_CODE && method.methodType() != QMetaMethod::Signal)) {
    return false;
  }
  return true;
}

static QByteArray normalize(QObject *obj, const char *signature)
{
  int idx = obj->metaObject()->indexOfMethod(signature + 1);
  if (idx == -1) {
    return QMetaObject::normalizedSignature(signature);
  } else {
    // oh how I'd like to use ::fromRawData here reliably ;-)
    return QByteArray(signature);
  }
}

ConnectionModel::ConnectionModel(QObject *parent)
  : QAbstractTableModel(parent)
{
  qRegisterMetaType<const char*>("const char*");
  qRegisterMetaType<Qt::ConnectionType>("Qt::ConnectionType");
  qRegisterMetaType<Connection>();
}

void ConnectionModel::connectionAdded(QObject *sender, const char *signal,
                                      QObject *receiver, const char *method,
                                      Qt::ConnectionType type)
{
  if (sender == this || receiver == this || !sender || !receiver) {
    return;
  }

  Connection c;
  c.sender = sender;
  c.signal = normalize(sender, signal);
  c.receiver = receiver;
  c.method = normalize(receiver, method);
  c.type = type;
  c.location = SignalSlotsLocationStore::extractLocation(signal);

  // check if that's actually a valid connection
  if (checkMethodForObject(sender, c.signal, true) &&
      checkMethodForObject(receiver, c.method, false)) {
    c.valid = QMetaObject::checkConnectArgs(c.signal, c.method);
  } else {
    c.valid = false;
  }
  //TODO: we could check more stuff here  eg. if queued connection is possible etc.
  //and use verktygs heuristics to detect likely misconnects

  // when called from background, delay into foreground, otherwise call directly
  QMetaObject::invokeMethod(this, "connectionAddedMainThread", Qt::AutoConnection,
                            Q_ARG(GammaRay::Connection, c));
}

void ConnectionModel::connectionAddedMainThread(const Connection& connection)
{
  Q_ASSERT(thread() == QThread::currentThread());

  {
    ReadOrWriteLocker objectLock(Probe::instance()->objectLock());
    if (!Probe::instance()->isValidObject(connection.sender) ||
        !Probe::instance()->isValidObject(connection.receiver)) {
      return;
    }
  }

  beginInsertRows(QModelIndex(), m_connections.size(), m_connections.size());
  m_connections.push_back(connection);
  endInsertRows();
}

void ConnectionModel::connectionRemoved(QObject *sender, const char *signal,
                                        QObject *receiver, const char *method)
{
  if (sender == this || receiver == this) {
    return;
  }

  QByteArray normalizedSignal, normalizedMethod;
  if (signal) {
    normalizedSignal = QMetaObject::normalizedSignature(signal);
  }
  if (method) {
    normalizedMethod = QMetaObject::normalizedSignature(method);
  }

  // when called from background, delay into foreground, otherwise call directly
  QMetaObject::invokeMethod(this, "connectionRemovedMainThread", Qt::AutoConnection,
                            Q_ARG(QObject*, sender), Q_ARG(QByteArray, normalizedSignal),
                            Q_ARG(QObject*, receiver), Q_ARG(QByteArray, normalizedMethod));
}

void ConnectionModel::connectionRemovedMainThread(QObject *sender, const QByteArray &normalizedSignal,
                                                  QObject *receiver, const QByteArray &normalizedMethod)
{
  Q_ASSERT(thread() == QThread::currentThread());

  for (int i = 0; i < m_connections.size();) {
    bool remove = false;

    const Connection &con = m_connections.at(i);
    if (!con.receiver || !con.sender) {
      // might be invalidated from a background thread
      remove = true;
    } else if ((sender == 0 || con.sender == sender) &&
        (receiver == 0 || con.receiver == receiver) &&
        (normalizedSignal.isEmpty() || con.signal == normalizedSignal) &&
        (normalizedMethod.isEmpty() || con.method == normalizedMethod)) {
      // the connection was actually removed
      remove = true;
    }

    if (remove) {
      // note: QVector in Qt4 does not move objects when erasing, even though Connection is marked as movable.
      // To workaround this issue, we swap with the last entry and pop from the back which is cheap.
      if (i < m_connections.size() - 1) {
        qSwap(m_connections[i], m_connections.last());
        emit dataChanged(index(i, 0), index(i, columnCount()));
      }

      beginRemoveRows(QModelIndex(), m_connections.size() - 1, m_connections. size() - 1);
      m_connections.pop_back();
      endRemoveRows();
    } else {
      ++i;
    }
  }
}

QVariant ConnectionModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || index.row() < 0 || index.row() >= m_connections.size()) {
    return QVariant();
  }

  Connection con = m_connections.at(index.row());

  ReadOrWriteLocker probeLock(Probe::instance()->objectLock());
  if (!Probe::instance()->isValidObject(con.sender)) {
    con.sender = 0;
  }
  if (!Probe::instance()->isValidObject(con.receiver)) {
    con.receiver = 0;
  }

  if (role == Qt::DisplayRole) {
    if (index.column() == 0) {
      if (con.sender) {
        return Util::displayString(con.sender);
      } else {
        return QLatin1String("<destroyed>");
      }
    }
    if (index.column() == 1) {
      return con.signal.mid(1);
    }
    if (index.column() == 2) {
      if (con.receiver) {
        return Util::displayString(con.receiver);
      } else {
        return QLatin1String("<destroyed>");
      }
    }
    if (index.column() == 3) {
      return con.method.mid(1);
    }
    if (index.column() == 4) {
      switch (con.type) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
      case Qt::AutoCompatConnection:
        return QLatin1String("AutoCompatConnection");
#endif
      case Qt::AutoConnection:
        return QLatin1String("AutoConnection");
      case Qt::BlockingQueuedConnection:
        return QLatin1String("BlockingQueuedConnection");
      case Qt::DirectConnection:
        return QLatin1String("DirectConnection");
      case Qt::QueuedConnection:
        return QLatin1String("QueuedConnection");
      case Qt::UniqueConnection:
        return QLatin1String("UniqueConnection");
      default:
        return tr("Unknown connection type: %1").arg(con.type);
      }
    }
    if (index.column() == 5) {
      return con.location;
    }
  } else if (role == SenderRole) {
    return QVariant::fromValue(con.sender);
  } else if (role == ReceiverRole) {
    return QVariant::fromValue(con.receiver);
  } else if (role == Qt::ForegroundRole) {
    if (!con.valid) {
      return QVariant::fromValue<QColor>(Qt::red);
    }
  } else if (role == ConnectionValidRole) {
    return con.valid;
  }
  return QVariant();
}

QVariant ConnectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0:
        return tr("Sender");
      case 1:
        return tr("Signal");
      case 2:
        return tr("Receiver");
      case 3:
        return tr("Method");
      case 4:
        return tr("Connection Type");
      case 5:
        return tr("Location");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

int ConnectionModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 6;
}

int ConnectionModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return m_connections.size();
}

