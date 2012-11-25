#ifndef GAMMARAY_SERVER_H
#define GAMMARAY_SERVER_H

#include <network/endpoint.h>

class QTcpServer;

namespace GammaRay {

class RemoteModelServer;

class Server : public Endpoint
{
  Q_OBJECT
  public:
    explicit Server(QObject *parent = 0);
    ~Server();

  private slots:
    void newConnection();

  private:
    QTcpServer *m_tcpServer;
    RemoteModelServer *m_modelServer;
};

}

#endif
