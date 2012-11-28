#ifndef GAMMARAY_ENDPOINT_H
#define GAMMARAY_ENDPOINT_H

#include "protocol.h"

#include <QObject>
#include <QPointer>

class QIODevice;
class QDataStream;

namespace GammaRay {

class Message;

/** Network protocol endpoint. */
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

private slots:
  void readyRead();
  void connectionClosed();

private:
  static Endpoint *s_instance;
  QPointer<QIODevice> m_socket;
  QScopedPointer<QDataStream> m_stream;

  QMap<QString, Protocol::ObjectAddress> m_objectsAddresses;
};
}

#endif // GAMMARAY_ENDPOINT_H
