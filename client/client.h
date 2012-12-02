#ifndef GAMMARAY_CLIENT_H
#define GAMMARAY_CLIENT_H

#include <network/protocol.h>
#include <network/endpoint.h>

namespace GammaRay {

class RemoteModel;

class Client : public Endpoint
{
  Q_OBJECT
public:
  explicit Client(QObject *parent = 0);
  ~Client();

  void connectToHost( /* TODO */ );

  void registerForObject(Protocol::ObjectAddress &objectAddress, QObject *handler, const char* slot);

  static Client* instance();

protected:
    void messageReceived(const Message& msg);
};

}

#endif // GAMMARAY_CLIENT_H
