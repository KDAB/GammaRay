#ifndef GAMMARAY_CLIENT_H
#define GAMMARAY_CLIENT_H

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

  // ### temporary
  void setModel(RemoteModel* model);

  void registerForObject(const QString &serverObject, QObject *handler, const char* slot);

protected:
    void messageReceived(const Message& msg);

private:
  RemoteModel *m_model;
};

}

#endif // GAMMARAY_CLIENT_H
