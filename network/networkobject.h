#ifndef GAMMARAY_NETWORKOBJECT_H
#define GAMMARAY_NETWORKOBJECT_H

#include "protocol.h"

#include <QObject>

namespace GammaRay {

class Message;

/** Base class for objects that forwards signals bidirectionally over the network.
 *  Do not create directly, instead retrieve instances via GammaRay::ObjectBroker.
 *  TODO: add support for signal arguments
 */
class NetworkObject : public QObject
{
  Q_OBJECT
public:
  explicit NetworkObject(const QString &objectName, QObject* parent);
  ~NetworkObject();

  /** Invoke methods connected to @p signalName on the other end. */
  void emitSignal(const QString &signalName);

  // TODO publish signals

  /** @p slot on @p receiver will be called whenever the remote object ends @p singnalName. */
  void subscribeToSignal(const QString &signalName, QObject* receiver, const char* slot);

protected:
  QString m_objectName;
  Protocol::ObjectAddress m_myAddress;

private slots:
  void newMessage(const GammaRay::Message &msg);

private:
  QHash<QString, QPair<QObject*, QByteArray> > m_subscriptions;
};

}

#endif // GAMMARAY_NETWORKOBJECT_H
