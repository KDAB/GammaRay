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

  // TODO connect all signals
  connect(m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(dataChanged(QModelIndex,QModelIndex)));
  connect(m_model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), SLOT(headerDataChanged(Qt::Orientation,int,int)));
  connect(m_model, SIGNAL(rowsInserted(QModelIndex,int, int)), SLOT(rowsInserted(QModelIndex,int,int)));
  connect(m_model, SIGNAL(rowsRemoved(QModelIndex,int,int)), SLOT(rowsRemoved(QModelIndex,int,int)));
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
      qDebug() << "row col count for" << index << qmIndex;

      Message msg(m_myAddress);
      msg.stream() << Protocol::ModelRowColumnCountReply << index << m_model->rowCount(qmIndex) << m_model->columnCount(qmIndex);
      Server::stream() << msg;
      break;
    }

    case Protocol::ModelContentRequest:
    {
      Protocol::ModelIndex index;
      msg.stream() >> index;
      const QModelIndex qmIndex = Protocol::toQModelIndex(m_model, index);
      if (!qmIndex.isValid())
        break;

      Message msg(m_myAddress);
      msg.stream() << Protocol::ModelContentReply << index << m_model->itemData(qmIndex);
      Server::stream() << msg;
      break;
    }

    case Protocol::ModelHeaderRequest:
    {
      qint8 orientation;
      qint32 section;
      msg.stream() >> orientation >> section;

      QHash<qint32, QVariant> data;
      data.insert(Qt::DisplayRole, m_model->headerData(section, static_cast<Qt::Orientation>(orientation), Qt::DisplayRole)); // TODO: add all roles

      Message msg(m_myAddress);
      msg.stream() << Protocol::ModelHeaderReply << orientation << section << data;
      Server::stream() << msg;
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
      Message reply(m_myAddress);
      reply.stream() << Protocol::ModelSyncBarrier << barrierId;
      Server::stream() << reply;
      break;
    }
  }
}

void RemoteModelServer::dataChanged(const QModelIndex& begin, const QModelIndex& end)
{
  // TODO check if somebody is listening (here or in Server?)
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress);
  msg.stream() << Protocol::ModelContentChanged << Protocol::fromQModelIndex(begin) << Protocol::fromQModelIndex(end);
  Server::stream() << msg;
}

void RemoteModelServer::headerDataChanged(Qt::Orientation orientation, int first, int last)
{
  if (!Server::isConnected())
    return;
  // TODO
}

void RemoteModelServer::rowsInserted(const QModelIndex& parent, int start, int end)
{
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress);
  msg.stream() << Protocol::ModelRowsAdded << Protocol::fromQModelIndex(parent) << start << end;
  Server::stream() << msg;
}

void RemoteModelServer::rowsRemoved(const QModelIndex& parent, int start, int end)
{
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress);
  msg.stream() << Protocol::ModelRowsRemoved << Protocol::fromQModelIndex(parent) << start << end;
  Server::stream() << msg;
}

void RemoteModelServer::layoutChanged()
{
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress);
  msg.stream() << Protocol::ModelLayoutChanged;
  Server::stream() << msg;
}

void RemoteModelServer::modelReset()
{
  if (!Server::isConnected())
    return;
  Message msg(m_myAddress);
  msg.stream() << Protocol::ModelReset;
  Server::stream() << msg;
}

#include "remotemodelserver.moc"
