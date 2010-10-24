#ifndef ENDOSCOPE_CONNECTIONMODEL_H
#define ENDOSCOPE_CONNECTIONMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QVector>
#include <QtCore/qsharedpointer.h>

namespace Endoscope {

class ConnectionModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    enum Role {
      SenderRole = Qt::UserRole + 1,
      ReceiverRole
    };
    explicit ConnectionModel(QObject* parent = 0);

    void connectionAdded( QObject* sender, const char* signal, QObject* receiver, const char* method, Qt::ConnectionType type );
    void connectionRemoved( QObject* sender, const char* signal, QObject* receiver, const char* method );

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

  private:
    struct Connection
    {
      QObject *rawSender;
      QWeakPointer<QObject> sender;
      QByteArray signal;
      QObject *rawReceiver;
      QWeakPointer<QObject> receiver;
      QByteArray method;
      Qt::ConnectionType type;
    };
    QVector<Connection> m_connections;
};

}

#endif // ENDOSCOPE_CONNECTIONMODEL_H
