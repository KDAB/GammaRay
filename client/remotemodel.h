#ifndef GAMMARAY_REMOTEMODEL_H
#define GAMMARAY_REMOTEMODEL_H

#include <network/protocol.h>

#include <QAbstractItemModel>
#include <QVector>

namespace GammaRay {

class Message;

class RemoteModel : public QAbstractItemModel
{
  Q_OBJECT
  public:
    explicit RemoteModel(const QString &serverObject, QObject *parent = 0);
    ~RemoteModel();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  public slots:
    void newMessage(const GammaRay::Message &msg);

  private:
    struct Node { // represents one row
      Node() : parent(0), rowCount(-1), columnCount(-1) {}
      Node* parent;
      QVector<Node*> children;
      qint32 rowCount;
      qint32 columnCount;
      QHash<int, QHash<int, QVariant> > data; // column -> role -> data
      QHash<int, int> flags;                  // column -> flags
    };

    Node* nodeForIndex(const QModelIndex &index) const;
    Node* nodeForIndex(const Protocol::ModelIndex &index) const;
    QModelIndex modelIndexForNode(GammaRay::RemoteModel::Node* node, int column) const;

    void requestRowColumnCount(const QModelIndex &index) const;
    void requestDataAndFlags(const QModelIndex &index) const;
    void requestHeaderData(Qt::Orientation orientation, int section) const;

    Node* m_root;

    mutable QHash<Qt::Orientation, QHash<int, QHash<int, QVariant> > > m_headers; // orientation -> section -> role -> data

    QString m_serverObject;
    Protocol::ObjectAddress m_myAddress;
};

}

#endif
