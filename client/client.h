#ifndef GAMMARAY_CLIENT_H
#define GAMMARAY_CLIENT_H

#include <network/protocol.h>
#include <network/endpoint.h>

namespace GammaRay {

class RemoteModel;

/** Client-side connection endpoint. */
class Client : public Endpoint
{
  Q_OBJECT
public:
  explicit Client(QObject *parent = 0);
  ~Client();

  /** Connect to @p hostName on port @p port. */
  void connectToHost(const QString &hostName, quint16 port);

  /** Register a message handler for @p objectAddress on object @p handler.
   *  Once a message for this object is received, @p slot is called.
   */
  void registerForObject(Protocol::ObjectAddress objectAddress, QObject *handler, const char* slot);

  /** Unregister the message handler for @p objectAddress. */
  void unregisterForObject(Protocol::ObjectAddress objectAddress);

  /** Singleton accessor. */
  static Client* instance();

protected:
  void messageReceived(const Message& msg);
  void handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName);

private:
  void unmonitorObject(Protocol::ObjectAddress objectAddress);

private slots:
  void socketConnected();

private:
  bool m_versionChecked;
};

}

#endif // GAMMARAY_CLIENT_H
