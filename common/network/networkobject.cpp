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

  QByteArray signalName;
  msg.payload() >> signalName;
  Q_ASSERT(!signalName.isEmpty());

  QVariantList args;
  msg.payload() >> args;

  emitLocal(signalName, args);
}

void NetworkObject::emitSignal(const char *signalName)
{
  const QByteArray name(signalName);
  Q_ASSERT(!name.isEmpty());

  if (Endpoint::isConnected()) {
    Message msg(m_myAddress, Protocol::MethodCall);
    msg.payload() << name;
    Endpoint::send(msg);
  }

  // emit locally as well, for in-process mode
  emitLocal(signalName, QVariantList());
}

void NetworkObject::subscribeToSignal(const char *signalName, QObject* receiver, const char* slot)
{
  m_subscriptions.insert(signalName, qMakePair(receiver, QByteArray(slot)));
}

void NetworkObject::emitLocal(const char* signalName, const QVariantList& args)
{
  const QHash<QByteArray, QPair<QObject*, QByteArray> >::const_iterator it = m_subscriptions.constFind(signalName);
  if (it == m_subscriptions.constEnd())
    return;

  // TODO add args, requires we have access to GammaRay::MethodArgument here
  QMetaObject::invokeMethod(it.value().first, it.value().second);
}

#include "networkobject.moc"
