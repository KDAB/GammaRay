/*
  outboundconnectionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "outboundconnectionsmodel.h"

#include <core/util.h>

#include <QDebug>
#include <QMetaMethod>
#include <private/qobject_p.h>

using namespace GammaRay;

static QString displayString(const QMetaMethod &method)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  return method.methodSignature();
#else
  return method.signature();
#endif
}

OutboundConnectionsModel::OutboundConnectionsModel(QObject* parent):
  QAbstractTableModel(parent)
{
}

OutboundConnectionsModel::~OutboundConnectionsModel()
{
}

void OutboundConnectionsModel::setObject(QObject* object)
{
  beginResetModel();
  m_connections.clear();
  m_object = object;

  if (!object) {
    return;
    endResetModel();
  }

  QObjectPrivate *d = QObjectPrivate::get(object);
  if (d->connectionLists) {
    // HACK: the declaration of d->connectionsLists is not accessible for us...
    const QVector<QObjectPrivate::ConnectionList> *cl = reinterpret_cast<QVector<QObjectPrivate::ConnectionList>*>(d->connectionLists);
    for (int signalIndex = 0; signalIndex < cl->count(); ++signalIndex) {
      const QObjectPrivate::Connection *c = cl->at(signalIndex).first;
      while (c) {
        if (!c->receiver) {
          c = c->nextConnectionList;
          continue;
        }

        Connection conn;
        conn.receiver = c->receiver;
        conn.signalIndex = signalIndex;
        conn.slotIndex = c->method();
        c = c->nextConnectionList;
        m_connections.push_back(conn);
      }
    }
  }

  endResetModel();
}

QVariant OutboundConnectionsModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || !m_object)
    return QVariant();

  if (role == Qt::DisplayRole) {
    const Connection &conn = m_connections.at(index.row());
    switch (index.column()) {
      case 0:
      {
        if (conn.signalIndex < 0)
          return tr("<unknown>");
        const QMetaMethod signal = m_object->metaObject()->method(conn.signalIndex);
        return displayString(signal);
      }
      case 1:
        if (!conn.receiver)
          return tr("<destroyed>");
        return Util::displayString(conn.receiver);
      case 2:
      {
        if (!conn.receiver)
          return tr("<destroyed>");
        const QMetaMethod slot = conn.receiver->metaObject()->method(conn.slotIndex);
        return displayString(slot);
      }
    }
  }

  return QVariant();
}

int OutboundConnectionsModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 3;
}

int OutboundConnectionsModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return m_connections.size();
}

QVariant OutboundConnectionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return tr("Signal");
      case 1: return tr("Receiver");
      case 2: return tr("Slot");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}
