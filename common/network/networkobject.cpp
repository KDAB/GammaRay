#include "networkobject.h"
#include "message.h"
#include "endpoint.h"
#include "methodargument.h"

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

void NetworkObject::emitSignal(const char *signalName, const QVariantList &args)
{
  const QByteArray name(signalName);
  Q_ASSERT(!name.isEmpty());

  if (Endpoint::isConnected()) {
    Message msg(m_myAddress, Protocol::MethodCall);
    msg.payload() << name << args;
    Endpoint::send(msg);
  }

  // emit locally as well, for in-process mode
  emitLocal(signalName, args);
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

  Q_ASSERT(args.size() <= 10);
  QVector<MethodArgument> a;
  a.reserve(10);
  foreach (const QVariant &v, args)
    a.push_back(MethodArgument(v));
  a.resize(10);

  QMetaObject::invokeMethod(it.value().first, it.value().second, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
}

#include "networkobject.moc"
