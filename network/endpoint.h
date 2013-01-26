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

  /** Send @p msg to the connected endpoint. */
  static void send(const Message &msg);
  /** Returns @c true if we are currently connected to another endpoint. */
  static bool isConnected();
  static quint16 defaultPort();

  /** Returns the object address for @p objectName, or @c Protocol::InvalidObjectAddress if not known. */
  Protocol::ObjectAddress objectAddress(const QString &objectName) const;

signals:
  /** Emitted when we lost the connection to the other endpoint. */
  void disconnected();

  /** Emitted when a new object with name @p objectName has been registered at address @p objectAddress. */
  void objectRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
  void objectUnregistered(const QString &objectName, Protocol::ObjectAddress objectAddress);

protected:
  Endpoint(QObject* parent = 0);
  /** Call with the socket once you have established a connection to another endpoint, takes ownership of @p device. */
  void setDevice(QIODevice* device);

  /** The object address of the other endpoint. */
  Protocol::ObjectAddress endpointAddress() const;

  /** Called for every incoming message.
   *  @see dispatchMessage().
   */
  virtual void messageReceived(const Message &msg) = 0;

  /** Call this when learning about a new object <-> address mapping. */
  void registerObjectInternal(const QString &objectName, Protocol::ObjectAddress objectAddress);
  /** Call this when learning about a dissolved object <-> address mapping. */
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
  Protocol::ObjectAddress m_myAddress;
};

}

#endif // GAMMARAY_ENDPOINT_H
