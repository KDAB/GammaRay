#ifndef GAMMARAY_CLIENT_H
#define GAMMARAY_CLIENT_H

#include <network/endpoint.h>

namespace GammaRay {

class Client : public Endpoint
{
  Q_OBJECT
public:
  explicit Client(QObject *parent = 0);
  ~Client();

  void connectToHost( /* TODO */ );
};

}

#endif // GAMMARAY_CLIENT_H
