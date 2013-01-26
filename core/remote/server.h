#ifndef GAMMARAY_SERVER_H
#define GAMMARAY_SERVER_H

#include <network/endpoint.h>

class QTcpServer;

namespace GammaRay {

class Server : public Endpoint
{
  Q_OBJECT
  public:
    explicit Server(QObject *parent = 0);
    ~Server();

    Protocol::ObjectAddress registerObject(const QString &objectName, QObject* receiver, const char* messageHandlerName);
    static Server* instance();

  protected:
    void messageReceived(const Message& msg);
    void handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName);

  private slots:
    void newConnection();

  private:
    QTcpServer *m_tcpServer;

    Protocol::ObjectAddress m_nextAddress;
};

}

#endif
