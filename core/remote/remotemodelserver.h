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
    void sendAddRemoveMessage(Protocol::MessageType type, const QModelIndex &parent, int start, int end);
    void sendMoveMessage(Protocol::MessageType type, const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationIndex);

  private slots:
    void dataChanged(const QModelIndex &begin, const QModelIndex &end);
    void headerDataChanged(Qt::Orientation orientation, int first, int last);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    void columnsInserted(const QModelIndex &parent, int start, int end);
    void columnsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationColumn);
    void columnsRemoved(const QModelIndex &parent, int start, int end);
    void layoutChanged();
    void modelReset();

  private:
    QAbstractItemModel *m_model;
    Protocol::ObjectAddress m_myAddress;
};

}

#endif
