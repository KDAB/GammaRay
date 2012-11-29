#ifndef GAMMARAY_REMOTEMODELSERVER_H
#define GAMMARAY_REMOTEMODELSERVER_H

#include <network/protocol.h>

#include <QObject>

class QAbstractItemModel;

namespace GammaRay {

class Message;

class RemoteModelServer : public QObject
{
  Q_OBJECT
  public:
    explicit RemoteModelServer(const QString &objectName, QObject *parent = 0);
    ~RemoteModelServer();

    void setModel(QAbstractItemModel *model);

  public slots:
    void newRequest(const GammaRay::Message &msg);

  private:
    QAbstractItemModel *m_model;
    Protocol::ObjectAddress m_myAddress;
};

}

#endif
