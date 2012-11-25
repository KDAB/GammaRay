#ifndef GAMMARAY_REMOTEMODELSERVER_H
#define GAMMARAY_REMOTEMODELSERVER_H

#include <QObject>

class QAbstractItemModel;
class QDataStream;

namespace GammaRay {

class RemoteModelServer : public QObject
{
  Q_OBJECT
  public:
    explicit RemoteModelServer(QObject *parent = 0);
    ~RemoteModelServer();

    void setModel(QAbstractItemModel *model);
    void setStream(QDataStream *stream);

  public slots:
    void newRequest();

  private:
    QAbstractItemModel *m_model;
    QDataStream *m_stream;
};

}

#endif
