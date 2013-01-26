#include "remotemodelserver.h"
#include "server.h"
#include <network/protocol.h>
#include <network/message.h>

#include <QAbstractItemModel>
#include <QDataStream>
#include <QDebug>

using namespace GammaRay;

RemoteModelServer::RemoteModelServer(const QString &objectName, QObject *parent) :
  QObject(parent),
  m_model(0)
{
  m_myAddress = Server::instance()->registerObject(objectName, this, "newRequest");
}

RemoteModelServer::~RemoteModelServer()
{
}

void RemoteModelServer::setModel(QAbstractItemModel *model)
{
  // TODO send reset, disconnect old model
  m_model = model;

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
}

void RemoteModelServer::newRequest(const GammaRay::Message &msg)
{
  switch (msg.type()) {
    case Protocol::ModelRowColumnCountRequest:
    {
      Protocol::ModelIndex index;
      msg.stream() >> index;
      const QModelIndex qmIndex = Protocol::toQModelIndex(m_model, index);

      Message msg(m_myAddress, Protocol::ModelRowColumnCountReply);
      msg.stream() << index << m_model->rowCount(qmIndex) << m_model->columnCount(qmIndex);
      Server::send(msg);
      break;
    }

    case Protocol::ModelContentRequest:
    {
      Protocol::ModelIndex index;
      msg.stream() >> index;
      const QModelIndex qmIndex = Protocol::toQModelIndex(m_model, index);
      if (!qmIndex.isValid())
        break;

      Message msg(m_myAddress, Protocol::ModelContentReply);
      msg.stream() << index << m_model->itemData(qmIndex) << qint32(m_model->flags(qmIndex));
      Server::send(msg);
      break;
    }

    case Protocol::ModelHeaderRequest:
    {
      qint8 orientation;
      qint32 section;
      msg.stream() >> orientation >> section;

      QHash<qint32, QVariant> data;
      data.insert(Qt::DisplayRole, m_model->headerData(section, static_cast<Qt::Orientation>(orientation), Qt::DisplayRole)); // TODO: add all roles

      Message msg(m_myAddress, Protocol::ModelHeaderReply);
      msg.stream() << orientation << section << data;
      Server::send(msg);
      break;
    }

    case Protocol::ModelSetDataRequest:
    {
      Protocol::ModelIndex index;
      int role;
      QVariant value;
      msg.stream() >> index >> role >> value;

      m_model->setData(Protocol::toQModelIndex(m_model, index), value, role);
      break;
    }

    case Protocol::ModelSyncBarrier:
    {
      qint32 barrierId;
      msg.stream() >> barrierId;
      Message reply(m_myAddress, Protocol::ModelSyncBarrier);
      reply.stream() << barrierId;
      Server::send(reply);
      break;
    }
  }
}

void RemoteModelServer::dataChanged(const QModelIndex& begin, const QModelIndex& end)
{
  // TODO check if somebody is listening (here or in Server?)
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress, Protocol::ModelContentChanged);
  msg.stream() << Protocol::fromQModelIndex(begin) << Protocol::fromQModelIndex(end);
  Server::send(msg);
}

void RemoteModelServer::headerDataChanged(Qt::Orientation orientation, int first, int last)
{
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress, Protocol::ModelHeaderChanged);
  msg.stream() <<  qint8(orientation) << first << last;
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
  msg.stream() << Protocol::fromQModelIndex(parent) << start << end;
  Server::send(msg);

}

void RemoteModelServer::sendMoveMessage(Protocol::MessageType type, const QModelIndex& sourceParent, int sourceStart, int sourceEnd,
                                        const QModelIndex& destinationParent, int destinationIndex)
{
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress, type);
  msg.stream() << Protocol::fromQModelIndex(sourceParent) << qint32(sourceStart) << qint32(sourceEnd)
               << Protocol::fromQModelIndex(destinationParent) << qint32(destinationIndex);
  Server::send(msg);
}

#include "remotemodelserver.moc"
