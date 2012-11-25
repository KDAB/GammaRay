#ifndef GAMMARAY_REMOTEMODEL_H
#define GAMMARAY_REMOTEMODEL_H

#include <remote/protocol.h>

#include <QAbstractItemModel>
#include <QVector>

class QDataStream;

namespace GammaRay {

class RemoteModel : public QAbstractItemModel
{
  Q_OBJECT
  public:
    explicit RemoteModel(QObject *parent = 0);
    ~RemoteModel();

    void setStream(QDataStream *stream);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  public slots:
    void newMessage();

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

    Node* m_root;

    mutable QDataStream *m_stream;
};

}

#endif
