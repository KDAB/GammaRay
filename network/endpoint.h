#ifndef GAMMARAY_ENDPOINT_H
#define GAMMARAY_ENDPOINT_H

#include "protocol.h"

#include <QObject>
#include <QPointer>

class QIODevice;
class QDataStream;

namespace GammaRay {

class Message;

/** Network protocol endpoint.
 *  Contains:
 *  - object address <-> object name mapping
 *  - message handler registration and message dispatching
 */
class Endpoint : public QObject
{
  Q_OBJECT
public:
  ~Endpoint();

  static QDataStream& stream();
  static bool isConnected();
  static quint16 defaultPort();

  Protocol::ObjectAddress objectAddress(const QString &objectName) const;

signals:
  void disconnected();

  void objectRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
  void objectUnregistered(const QString &objectName, Protocol::ObjectAddress objectAddress);

protected:
  Endpoint(QObject* parent = 0);
  /// takes ownership
  void setDevice(QIODevice* device);

  virtual void messageReceived(const Message &msg) = 0;

  void registerObjectInternal(const QString &objectName, Protocol::ObjectAddress objectAddress);
  void unregisterObjectInternal(const QString& objectName);

  void registerMessageHandlerInternal(Protocol::ObjectAddress objectAddress, QObject *receiver, const char* messageHandlerName);

  void dispatchMessage(const GammaRay::Message& msg);

protected:
  QMap<QString, Protocol::ObjectAddress> objectAddresses() const;
  static Endpoint *s_instance;

  // TODO make private and move the object monitoring from server here as well
  QHash<Protocol::ObjectAddress, QPair<QObject*, QByteArray> > m_messageHandlers;

private slots:
  void readyRead();
  void connectionClosed();

private:
  QMap<QString, Protocol::ObjectAddress> m_objectAddresses;
  QPointer<QIODevice> m_socket;
  QScopedPointer<QDataStream> m_stream;
};

}

#endif // GAMMARAY_ENDPOINT_H
