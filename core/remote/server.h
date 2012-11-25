#ifndef GAMMARAY_SERVER_H
#define GAMMARAY_SERVER_H

#include <QObject>

class QTcpServer;

namespace GammaRay {

class RemoteModelServer;

class Server : public QObject
{
  Q_OBJECT
  public:
    explicit Server(QObject *parent = 0);
    ~Server();

  private slots:
    void newConnection();

  private:
    QTcpServer *m_tcpServer;
    QDataStream *m_stream;
    RemoteModelServer *m_modelServer;
};

}

#endif
