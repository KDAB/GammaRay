#include "networkobject.h"
#include "message.h"
#include "endpoint.h"

using namespace GammaRay;

NetworkObject::NetworkObject(const QString& objectName, QObject* parent) :
  QObject(parent),
  m_objectName(objectName),
  m_myAddress(Protocol::InvalidObjectAddress)
{
  // TODO register
}

NetworkObject::~NetworkObject()
{
}

void NetworkObject::newMessage(const Message& msg)
{
  Q_ASSERT(msg.type() == Protocol::MethodCall);

  QString signalName;
  msg.payload() >> signalName;
  Q_ASSERT(!signalName.isEmpty());

  const QHash<QString, QPair<QObject*, QByteArray> >::const_iterator it = m_subscriptions.constFind(signalName);
  if (it == m_subscriptions.constEnd())
    return;

  QMetaObject::invokeMethod(it.value().first, it.value().second);
}

void NetworkObject::emitSignal(const QString& signalName)
{
  Q_ASSERT(!signalName.isEmpty());
  Message msg(m_myAddress, Protocol::MethodCall);
  msg.payload() << signalName;
  Endpoint::send(msg);
}

void NetworkObject::subscribeToSignal(const QString& signalName, QObject* receiver, const char* slot)
{
  m_subscriptions.insert(signalName, qMakePair(receiver, QByteArray(slot)));
}

#include "networkobject.moc"
