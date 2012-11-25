#include "remotemodelserver.h"
#include <network/protocol.h>
#include <network/message.h>

#include <QAbstractItemModel>
#include <QDataStream>
#include <QDebug>

using namespace GammaRay;

RemoteModelServer::RemoteModelServer(QObject *parent) : 
  QObject(parent),
  m_model(0),
  m_stream(0)
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

void RemoteModelServer::setStream(QDataStream *stream)
{
  m_stream = stream;

  if (m_stream->device()->bytesAvailable())
    newRequest();
}

void RemoteModelServer::newRequest()
{
  qDebug() << Q_FUNC_INFO;

  qint32 req;
  *m_stream >> req;

  qDebug() << req << m_stream->status();

  switch (req) {
    case Protocol::RowColumnCountRequest:
    {
      Protocol::ModelIndex index;
      *m_stream >> index;
      const QModelIndex qmIndex = Protocol::toQModelIndex(m_model, index);
      qDebug() << "row col count for" << index << qmIndex << m_stream->status();

      Message msg;
      msg.stream() << qint32(Protocol::RowColumnCountReply) << index << m_model->rowCount(qmIndex) << m_model->columnCount(qmIndex);
      *m_stream << msg;
      break;
    }
    case Protocol::ContentRequest:
    {
      Protocol::ModelIndex index;
      *m_stream >> index;
      const QModelIndex qmIndex = Protocol::toQModelIndex(m_model, index);
      if (!qmIndex.isValid())
        break;

      Message msg;
      msg.stream() << qint32(Protocol::ContentChanged) << index << m_model->itemData(qmIndex);
      *m_stream << msg;
    }
  }

  if (m_stream->device()->bytesAvailable()) // ### temporary
    newRequest(); 
}

#include "remotemodelserver.moc"
