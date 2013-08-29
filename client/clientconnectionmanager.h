#ifndef GAMMARAY_CLIENTCONNECTIONMANAGER_H
#define GAMMARAY_CLIENTCONNECTIONMANAGER_H

#include <QObject>

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
    void connectionEstablished();
    void connectionError(QAbstractSocket::SocketError error, const QString &msg);

    void toolModelPopulated();

  private:
    Client *m_client;
    MainWindow *m_mainWindow;
    QAbstractItemModel *m_toolModel;
};

}

#endif // GAMMARAY_CLIENTCONNECTIONMANAGER_H
