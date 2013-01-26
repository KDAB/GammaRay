#include "endpoint.h"
#include "message.h"

#include <QDebug>

using namespace GammaRay;

Endpoint* Endpoint::s_instance = 0;

Endpoint::Endpoint(QObject* parent): QObject(parent), m_socket(0)
{
  Q_ASSERT(!s_instance);
  s_instance = this;

  m_objectAddresses.insert(QLatin1String("com.kdab.GammaRay.Server"), Protocol::InvalidObjectAddress + 1);
}

Endpoint::~Endpoint()
{
}

void Endpoint::send(const Message& msg)
{
  Q_ASSERT(s_instance);
  msg.write(s_instance->m_socket);
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
  connect(m_socket.data(), SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_socket.data(), SIGNAL(disconnected()), SLOT(connectionClosed()));
  if (m_socket->bytesAvailable())
    readyRead();
}

void Endpoint::readyRead()
{
  qDebug() << Q_FUNC_INFO << m_socket->bytesAvailable();
  while (Message::canReadMessage(m_socket.data())) {
    messageReceived(Message::readMessage(m_socket.data()));
  }
}

void Endpoint::connectionClosed()
{
  qDebug() << Q_FUNC_INFO;
  m_socket->deleteLater();
  m_socket = 0;
  emit disconnected();
}

Protocol::ObjectAddress Endpoint::objectAddress(const QString& objectName) const
{
  const QMap<QString, Protocol::ObjectAddress>::const_iterator it = m_objectAddresses.constFind(objectName);
  if (it != m_objectAddresses.constEnd())
    return it.value();
  return Protocol::InvalidObjectAddress;
}

void Endpoint::registerObjectInternal(const QString& objectName, Protocol::ObjectAddress objectAddress)
{
  qDebug() << objectName << objectAddress;
  Q_ASSERT(!m_objectAddresses.contains(objectName));
  Q_ASSERT(!m_objectAddresses.values().contains(objectAddress));
  Q_ASSERT(objectAddress != Protocol::InvalidObjectAddress);

  m_objectAddresses.insert(objectName, objectAddress);
  emit objectRegistered(objectName, objectAddress);
}

void Endpoint::unregisterObjectInternal(const QString& objectName)
{
  Q_ASSERT(m_objectAddresses.contains(objectName));

  emit objectUnregistered(objectName, m_objectAddresses.value(objectName));
  m_objectAddresses.remove(objectName);
}

void Endpoint::registerMessageHandlerInternal(Protocol::ObjectAddress objectAddress, QObject* receiver, const char* messageHandlerName)
{
  m_messageHandlers.insert(objectAddress, qMakePair<QObject*, QByteArray>(receiver, messageHandlerName));
}

void Endpoint::dispatchMessage(const Message& msg)
{
  if (!m_messageHandlers.contains(msg.address()))
    return;
  QObject *receiver = m_messageHandlers.value(msg.address()).first;
  QByteArray method = m_messageHandlers.value(msg.address()).second;

  QMetaObject::invokeMethod(receiver, method, Q_ARG(GammaRay::Message, msg));
}

QMap< QString, Protocol::ObjectAddress > Endpoint::objectAddresses() const
{
  return m_objectAddresses;
}

#include "endpoint.moc"
