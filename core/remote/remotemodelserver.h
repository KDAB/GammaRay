#ifndef GAMMARAY_REMOTEMODELSERVER_H
#define GAMMARAY_REMOTEMODELSERVER_H

#include <network/protocol.h>

#include <QObject>

class QAbstractItemModel;

namespace GammaRay {

class Message;

/** Provides the server-side interface for a QAbstractItemModel to be used from a separate process. */
class RemoteModelServer : public QObject
{
  Q_OBJECT
  public:
    /** Registers a new model server object with name @p objectName (must be unique). */
    explicit RemoteModelServer(const QString &objectName, QObject *parent = 0);
    ~RemoteModelServer();

    /** Set the source model for this model server instance. */
    void setModel(QAbstractItemModel *model);

  public slots:
    void newRequest(const GammaRay::Message &msg);
    /** Notifications about an object on the client side (un)monitoring this object.
     *  If noone is watching, we don't send out any change notification to reduce network traffice.
     */
    void modelMonitored(bool monitored = false);

  private:
    void connectModel();
    void disconnectModel();
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
    bool m_monitored;
};

}

#endif
