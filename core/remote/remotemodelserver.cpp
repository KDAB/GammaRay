#include "remotemodelserver.h"
#include "server.h"
#include <network/protocol.h>
#include <network/message.h>

#include <QAbstractItemModel>
#include <QDataStream>
#include <QDebug>

using namespace GammaRay;

RemoteModelServer::RemoteModelServer(QObject *parent) : 
  QObject(parent),
  m_model(0)
{
}

RemoteModelServer::~RemoteModelServer()
{
}

void RemoteModelServer::setModel(QAbstractItemModel *model)
{
  // TODO send reset
  m_model = model;

  // TODO connect
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

      Message msg;
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

      Message msg;
      msg.stream() << Protocol::ModelContentReply << index << m_model->itemData(qmIndex);
      Server::stream() << msg;
    }

    case Protocol::ModelHeaderRequest:
    {
      qint8 orientation;
      qint32 section;
      msg.stream() >> orientation >> section;

      QHash<qint32, QVariant> data;
      data.insert(Qt::DisplayRole, m_model->headerData(section, static_cast<Qt::Orientation>(orientation), Qt::DisplayRole)); // TODO: add all roles

      Message msg;
      msg.stream() << Protocol::ModelHeaderChanged << orientation << section << data;
      Server::stream() << msg;
    }
  }
}

#include "remotemodelserver.moc"
