/*
  remotemodelserver.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "remotemodelserver.h"
#include "server.h"
#include <common/protocol.h>
#include <common/message.h>

#include <QAbstractItemModel>
#include <QDataStream>
#include <QBuffer>
#include <QIcon>

#include <iostream>

using namespace GammaRay;
using namespace std;

RemoteModelServer::RemoteModelServer(const QString &objectName, QObject *parent) :
  QObject(parent),
  m_model(0),
  m_dummyBuffer(new QBuffer(&m_dummyData, this)),
  m_monitored(false)
{
  setObjectName(objectName);
  m_myAddress = Server::instance()->registerObject(objectName, this, "newRequest", "modelMonitored");
  m_dummyBuffer->open(QIODevice::WriteOnly);
  connect(Server::instance(), SIGNAL(disconnected()), this, SLOT(modelMonitored()));
}

RemoteModelServer::~RemoteModelServer()
{
}

void RemoteModelServer::setModel(QAbstractItemModel *model)
{
  if (m_model) {
    disconnectModel();
    if (m_monitored)
      modelReset();
  }

  m_model = model;
  if (m_model && m_monitored)
    connectModel();
}

void RemoteModelServer::connectModel()
{
  Q_ASSERT(m_model);
  connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(dataChanged(QModelIndex,QModelIndex)));
  connect(m_model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), SLOT(headerDataChanged(Qt::Orientation,int,int)));
  connect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), SLOT(rowsInserted(QModelIndex,int,int)));
  connect(m_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), SLOT(rowsMoved(QModelIndex,int,int,QModelIndex,int)));
  connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), SLOT(rowsRemoved(QModelIndex,int,int)));
  connect(m_model, SIGNAL(columnsInserted(QModelIndex,int,int)), SLOT(columnsInserted(QModelIndex,int,int)));
  connect(m_model, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)), SLOT(columnsMoved(QModelIndex,int,int,QModelIndex,int)));
  connect(m_model, SIGNAL(columnsRemoved(QModelIndex,int,int)), SLOT(columnsRemoved(QModelIndex,int,int)));
  connect(m_model, SIGNAL(layoutChanged()), SLOT(layoutChanged()));
  connect(m_model, SIGNAL(modelReset()), SLOT(modelReset()));
  connect(m_model, SIGNAL(destroyed(QObject*)), SLOT(modelDeleted()));
}

void RemoteModelServer::disconnectModel()
{
  Q_ASSERT(m_model);
  disconnect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(dataChanged(QModelIndex,QModelIndex)));
  disconnect(m_model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), this, SLOT(headerDataChanged(Qt::Orientation,int,int)));
  disconnect(m_model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(rowsInserted(QModelIndex,int,int)));
  disconnect(m_model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(rowsMoved(QModelIndex,int,int,QModelIndex,int)));
  disconnect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(rowsRemoved(QModelIndex,int,int)));
  disconnect(m_model, SIGNAL(columnsInserted(QModelIndex,int,int)), this, SLOT(columnsInserted(QModelIndex,int,int)));
  disconnect(m_model, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(columnsMoved(QModelIndex,int,int,QModelIndex,int)));
  disconnect(m_model, SIGNAL(columnsRemoved(QModelIndex,int,int)), this, SLOT(columnsRemoved(QModelIndex,int,int)));
  disconnect(m_model, SIGNAL(layoutChanged()), this, SLOT(layoutChanged()));
  disconnect(m_model, SIGNAL(modelReset()), this, SLOT(modelReset()));
  disconnect(m_model, SIGNAL(destroyed(QObject*)), this, SLOT(modelDeleted()));
}

void RemoteModelServer::newRequest(const GammaRay::Message &msg)
{
  if (!m_model && msg.type() != Protocol::ModelSyncBarrier)
    return;

  switch (msg.type()) {
    case Protocol::ModelRowColumnCountRequest:
    {
      Protocol::ModelIndex index;
      msg.payload() >> index;
      const QModelIndex qmIndex = Protocol::toQModelIndex(m_model, index);

      Message msg(m_myAddress, Protocol::ModelRowColumnCountReply);
      msg.payload() << index << m_model->rowCount(qmIndex) << m_model->columnCount(qmIndex);
      Server::send(msg);
      break;
    }

    case Protocol::ModelContentRequest:
    {
      Protocol::ModelIndex index;
      msg.payload() >> index;
      const QModelIndex qmIndex = Protocol::toQModelIndex(m_model, index);
      if (!qmIndex.isValid())
        break;

      Message msg(m_myAddress, Protocol::ModelContentReply);
      msg.payload() << index << filterItemData(m_model->itemData(qmIndex)) << qint32(m_model->flags(qmIndex));
      Server::send(msg);
      break;
    }

    case Protocol::ModelHeaderRequest:
    {
      qint8 orientation;
      qint32 section;
      msg.payload() >> orientation >> section;
      Q_ASSERT(orientation == Qt::Horizontal || orientation == Qt::Vertical);
      Q_ASSERT(section >= 0);

      QHash<qint32, QVariant> data;
      // TODO: add all roles
      data.insert(Qt::DisplayRole, m_model->headerData(section, static_cast<Qt::Orientation>(orientation), Qt::DisplayRole));
      data.insert(Qt::ToolTipRole, m_model->headerData(section, static_cast<Qt::Orientation>(orientation), Qt::ToolTipRole));

      Message msg(m_myAddress, Protocol::ModelHeaderReply);
      msg.payload() << orientation << section << data;
      Server::send(msg);
      break;
    }

    case Protocol::ModelSetDataRequest:
    {
      Protocol::ModelIndex index;
      int role;
      QVariant value;
      msg.payload() >> index >> role >> value;

      m_model->setData(Protocol::toQModelIndex(m_model, index), value, role);
      break;
    }

    case Protocol::ModelSyncBarrier:
    {
      qint32 barrierId;
      msg.payload() >> barrierId;
      Message reply(m_myAddress, Protocol::ModelSyncBarrier);
      reply.payload() << barrierId;
      Server::send(reply);
      break;
    }
  }
}

QMap<int, QVariant> RemoteModelServer::filterItemData(const QMap< int, QVariant >& data) const
{
  QMap<int, QVariant> itemData(data);
  for (QMap<int, QVariant>::iterator it = itemData.begin(); it != itemData.end();) {
    if (it.value().userType() == qMetaTypeId<QIcon>()) {
      // see also: https://bugreports.qt-project.org/browse/QTBUG-33321
      const QIcon icon = it.value().value<QIcon>();
      ///TODO: what size to use? icon.availableSizes is empty...
      if (!icon.isNull())
        it.value() = icon.pixmap(QSize(16, 16));
      ++it;
    } else if (canSerialize(it.value())) {
      ++it;
    } else {
      it = itemData.erase(it);
    }
  }
  return itemData;
}

bool RemoteModelServer::canSerialize(const QVariant& value) const
{
  // ugly, but there doesn't seem to be a better way atm to find out without trying
  m_dummyBuffer->seek(0);
  QDataStream stream(m_dummyBuffer);
  return QMetaType::save(stream, value.userType(), value.constData());
}

void RemoteModelServer::modelMonitored(bool monitored)
{
  cout << Q_FUNC_INFO << ' ' << monitored << ' ' << static_cast<quint64>(m_myAddress) << endl;
  if (m_monitored == monitored)
    return;
  m_monitored = monitored;
  if (m_model) {
    if (m_monitored)
      connectModel();
    else
      disconnectModel();
  }
}

void RemoteModelServer::dataChanged(const QModelIndex& begin, const QModelIndex& end)
{
  // TODO check if somebody is listening (here or in Server?)
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress, Protocol::ModelContentChanged);
  msg.payload() << Protocol::fromQModelIndex(begin) << Protocol::fromQModelIndex(end);
  Server::send(msg);
}

void RemoteModelServer::headerDataChanged(Qt::Orientation orientation, int first, int last)
{
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress, Protocol::ModelHeaderChanged);
  msg.payload() <<  qint8(orientation) << first << last;
  Server::send(msg);
}

void RemoteModelServer::rowsInserted(const QModelIndex& parent, int start, int end)
{
  sendAddRemoveMessage(Protocol::ModelRowsAdded, parent, start, end);
}

void RemoteModelServer::rowsMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationRow)
{
  sendMoveMessage(Protocol::ModelRowsMoved, sourceParent, sourceStart, sourceEnd, destinationParent, destinationRow);
}

void RemoteModelServer::rowsRemoved(const QModelIndex& parent, int start, int end)
{
  sendAddRemoveMessage(Protocol::ModelRowsRemoved, parent, start, end);
}

void RemoteModelServer::columnsInserted(const QModelIndex& parent, int start, int end)
{
  sendAddRemoveMessage(Protocol::ModelColumnsAdded, parent, start, end);
}

void RemoteModelServer::columnsMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd, const QModelIndex& destinationParent, int destinationColumn)
{
  sendMoveMessage(Protocol::ModelColumnsMoved, sourceParent, sourceStart, sourceEnd, destinationParent, destinationColumn);
}

void RemoteModelServer::columnsRemoved(const QModelIndex& parent, int start, int end)
{
  sendAddRemoveMessage(Protocol::ModelColumnsRemoved, parent, start, end);
}

void RemoteModelServer::layoutChanged()
{
  if (!Server::isConnected())
    return;
  Server::send(Message(m_myAddress, Protocol::ModelLayoutChanged));
}

void RemoteModelServer::modelReset()
{
  if (!Server::isConnected())
    return;
  Server::send(Message(m_myAddress, Protocol::ModelReset));
}

void RemoteModelServer::sendAddRemoveMessage(Protocol::MessageType type, const QModelIndex& parent, int start, int end)
{
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress, type);
  msg.payload() << Protocol::fromQModelIndex(parent) << start << end;
  Server::send(msg);

}

void RemoteModelServer::sendMoveMessage(Protocol::MessageType type, const QModelIndex& sourceParent, int sourceStart, int sourceEnd,
                                        const QModelIndex& destinationParent, int destinationIndex)
{
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress, type);
  msg.payload() << Protocol::fromQModelIndex(sourceParent) << qint32(sourceStart) << qint32(sourceEnd)
               << Protocol::fromQModelIndex(destinationParent) << qint32(destinationIndex);
  Server::send(msg);
}

void RemoteModelServer::modelDeleted()
{
  m_model = 0;
  if (m_monitored)
    modelReset();
}

