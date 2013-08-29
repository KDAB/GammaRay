#ifndef GAMMARAY_CLIENTCONNECTIONMANAGER_H
#define GAMMARAY_CLIENTCONNECTIONMANAGER_H

#include <QObject>
#include <QAbstractSocket>
#include <QTime>

class QAbstractItemModel;

namespace GammaRay {

class Client;
class MainWindow;

/** Pre-MainWindow connection setup logic. */
class ClientConnectionManager : public QObject
{
  Q_OBJECT
  public:
    explicit ClientConnectionManager(QObject* parent = 0);
    ~ClientConnectionManager();

    void connectToHost(const QString &hostname, quint16 port);

  private slots:
    void connectToHost();
    void connectionEstablished();
    void connectionError(QAbstractSocket::SocketError error, const QString &msg);

    void toolModelPopulated();

  private:
    QString m_hostname;
    quint16 m_port;
    Client *m_client;
    MainWindow *m_mainWindow;
    QAbstractItemModel *m_toolModel;
    QTime m_connectionTimeout;
};

}

#endif // GAMMARAY_CLIENTCONNECTIONMANAGER_H
