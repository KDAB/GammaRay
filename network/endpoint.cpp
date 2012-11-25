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

bool Endpoint::isConnected()
{
  Q_ASSERT(s_instance);
  return s_instance->m_socket;
}

quint16 Endpoint::defaultPort()
{
  return 11732;
}

void Endpoint::setDevice(QIODevice* device)
{
  qDebug() << Q_FUNC_INFO << device;
  Q_ASSERT(!m_socket);
  Q_ASSERT(device);
  m_socket = device;
  m_stream.reset(new QDataStream(m_socket.data()));
  connect(m_socket.data(), SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_socket.data(), SIGNAL(disconnected()), SLOT(connectionClosed()));
  if (m_socket->bytesAvailable())
    readyRead();
}

void Endpoint::readyRead()
{
  qDebug() << Q_FUNC_INFO << m_socket->bytesAvailable();
  while (Message::canReadMessage(m_socket.data())) {
    Message msg;
    *m_stream >> msg;
    emit messageReceived(msg);
  }
}

void Endpoint::connectionClosed()
{
  qDebug() << Q_FUNC_INFO;
  m_socket->deleteLater();
  m_socket = 0;
  m_stream.reset();
  emit disconnected();
}

#include "endpoint.moc"
