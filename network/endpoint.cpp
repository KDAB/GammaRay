#include "endpoint.h"
#include "message.h"

#include <QDebug>

using namespace GammaRay;

Endpoint* Endpoint::s_instance = 0;

Endpoint::Endpoint(QObject* parent): QObject(parent), m_socket(0)
{
  Q_ASSERT(!s_instance);
  s_instance = this;
}

Endpoint::~Endpoint()
{
}

QDataStream& Endpoint::stream()
{
  Q_ASSERT(s_instance);
  Q_ASSERT(s_instance->m_stream);
  return *s_instance->m_stream;
}

quint16 Endpoint::defaultPort()
{
  return 11732;
}

void Endpoint::setDevice(QIODevice* device)
{
  qDebug() << Q_FUNC_INFO << device;
  Q_ASSERT(m_socket == 0);
  Q_ASSERT(device);
  m_socket = device;
  m_stream.reset(new QDataStream(m_socket));
  connect(m_socket, SIGNAL(readyRead()), SLOT(readyRead()));
  if (m_socket->bytesAvailable())
    readyRead();
}

void Endpoint::readyRead()
{
  qDebug() << Q_FUNC_INFO << m_socket->bytesAvailable();
  while (Message::canReadMessage(m_socket)) {
    Message msg;
    *m_stream >> msg;
    emit messageReceived(msg);
  }
}

#include "endpoint.moc"
