#ifndef GAMMARAY_SERVER_H
#define GAMMARAY_SERVER_H

#include <network/endpoint.h>

class QTcpServer;

namespace GammaRay {

/** Server side connection endpoint. */
class Server : public Endpoint
{
  Q_OBJECT
  public:
    explicit Server(QObject *parent = 0);
    ~Server();

    /** Register a new object with name @p objectName as a destination for messages.
     *  New messages to that object are passed to the slot @p messageHandlerName on @p receiver.
     *  If the object is unused on the client side it might be useful to disable sending out signals or
     *  other expensive operations, when this state changes the slot @p monitorNotifier is called.
     */
    Protocol::ObjectAddress registerObject(const QString &objectName, QObject* receiver, const char* messageHandlerName, const char* monitorNotifier = 0);

    /** Singleton accessor. */
    static Server* instance();

  protected:
    void messageReceived(const Message& msg);
    void handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName);

  private slots:
    void newConnection();

  private:
    QTcpServer *m_tcpServer;
    QHash<Protocol::ObjectAddress, QPair<QObject*, QByteArray> > m_monitorNotifiers;
    Protocol::ObjectAddress m_nextAddress;
};

}

#endif
