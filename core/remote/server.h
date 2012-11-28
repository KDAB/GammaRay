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

    Protocol::ObjectAddress registerObject(const QString &objectName, QObject* receiver, const char* messageHandlerName);

  protected:
    void messageReceived(const Message& msg);

  private slots:
    void newConnection();
    void objectDestroyed(QObject* object);

  private:
    QTcpServer *m_tcpServer;
    RemoteModelServer *m_modelServer;

    Protocol::ObjectAddress m_nextAddress;
    QHash<Protocol::ObjectAddress, QPair<QObject*, QByteArray> > m_messageHandlers;
    QHash<QObject*, QString> m_objectToNameMap;
};

}

#endif
