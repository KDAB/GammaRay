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

  void connectToHost(const QString &hostName, quint16 port);

  void registerForObject(Protocol::ObjectAddress &objectAddress, QObject *handler, const char* slot);

  static Client* instance();

protected:
    void messageReceived(const Message& msg);
    void handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName);
};

}

#endif // GAMMARAY_CLIENT_H
