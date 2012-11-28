#include "remotemodel.h"
#include "client.h"

#include <network/message.h>

#include <QDataStream>
#include <QDebug>

using namespace GammaRay;

RemoteModel::RemoteModel(QObject *parent) : 
  QAbstractItemModel(parent)
{
  m_root = new Node;
}

RemoteModel::~RemoteModel()
{
}

QModelIndex RemoteModel::index(int row, int column, const QModelIndex &parent) const
{
//   qDebug() << row << column << parent << rowCount(parent);
  Node *parentNode = nodeForIndex(parent);
  Q_ASSERT(parentNode->children.size() >= parentNode->rowCount);
  if (parentNode->rowCount <= row || parentNode->columnCount <= column)
    return QModelIndex();
  return createIndex(row, column, parentNode->children.at(row));
}

QModelIndex RemoteModel::parent(const QModelIndex &index) const
{
  Node *currentNode = nodeForIndex(index);
  Q_ASSERT(currentNode);
  if (currentNode == m_root || currentNode->parent == m_root)
    return QModelIndex();
  Q_ASSERT(currentNode->parent && currentNode->parent->parent);
  Q_ASSERT(currentNode->parent->children.contains(currentNode));
  Q_ASSERT(currentNode->parent->parent->children.contains(currentNode->parent));
  return createIndex(currentNode->parent->parent->children.indexOf(currentNode->parent), 0, currentNode->parent);
}

int RemoteModel::rowCount(const QModelIndex &index) const
{
  Node* node = nodeForIndex(index);
  Q_ASSERT(node);
  if (node->rowCount < 0) {
    requestRowColumnCount(index);
    return 0;
  }
  return node->rowCount;
}

int RemoteModel::columnCount(const QModelIndex &index) const
{
  Node* node = nodeForIndex(index);
  Q_ASSERT(node);
  if (node->columnCount < 0) {
    requestRowColumnCount(index);
    return 0;
  }
  return node->columnCount;
}

QVariant RemoteModel::data(const QModelIndex &index, int role) const
{
  Node* node = nodeForIndex(index);
  Q_ASSERT(node);

  if (node->data.contains(index.column())) {
    if (node->data.value(index.column()).contains(role))
      return node->data.value(index.column()).value(role);
    else if (role == Qt::DisplayRole)
      return tr("Loading...");
    return QVariant();
  }

  requestDataAndFlags(index);
  if (role == Qt::DisplayRole)
    return tr("Loading...");
  return QVariant();
}

bool RemoteModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  // TODO
  return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags RemoteModel::flags(const QModelIndex& index) const
{
  // TODO
  return QAbstractItemModel::flags(index);
}

QVariant RemoteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (!m_headers.contains(orientation) || !m_headers.value(orientation).contains(section))
    requestHeaderData(orientation, section);

  return m_headers.value(orientation).value(section).value(role);
}

void RemoteModel::newMessage(const GammaRay::Message& msg)
{
  switch (msg.type()) {
    case Protocol::ModelRowColumnCountReply:
    {
      Protocol::ModelIndex index;
      msg.stream() >> index;
      Node *node = nodeForIndex(index);
      Q_ASSERT(node->rowCount == -2 && node->columnCount == -1);
      msg.stream() >> node->rowCount >> node->columnCount;
      Q_ASSERT(node->rowCount >= 0 && node->columnCount >= 0);

      if (!node->rowCount || !node->columnCount)
        break;

      const QModelIndex qmi = modelIndexForNode(node, 0);
      qDebug() << "insert" << qmi << node->rowCount << node->columnCount;
      beginInsertRows(qmi, 0, node->rowCount - 1);
      beginInsertColumns(qmi, 0, node->columnCount - 1);
      node->children.reserve(node->rowCount);
      for (int i = 0; i < node->rowCount; ++i) {
        Node *child = new Node;
        child->parent = node;
        node->children.push_back(child);
      }
      endInsertColumns();
      endInsertRows();
      break;
    }

    case Protocol::ModelContentReply:
    {
      Protocol::ModelIndex index;
      msg.stream() >> index;
      Node *node = nodeForIndex(index);
      Q_ASSERT(node);
      qDebug() << "content reply" << index << node->data;
      typedef QMap<int, QVariant> ItemData;
      ItemData itemData;
      msg.stream() >> itemData;
      for (ItemData::const_iterator it = itemData.constBegin(); it != itemData.constEnd(); ++it) {
        node->data[index.last().second].insert(it.key(), it.value());
      }
      const QModelIndex qmi = modelIndexForNode(node, index.last().second);
      emit dataChanged(qmi, qmi);
      break;
    }

    case Protocol::ModelHeaderChanged:
    {
      qint8 orientation;
      qint32 section;
      QHash<qint32, QVariant> data;
      msg.stream() >> orientation >> section >> data;
      m_headers[static_cast<Qt::Orientation>(orientation)][section] = data;
      emit headerDataChanged(static_cast<Qt::Orientation>(orientation), section, section);
      break;
    }
  }
}

RemoteModel::Node* RemoteModel::nodeForIndex(const QModelIndex &index) const
{
  if (!index.isValid())
    return m_root;
  return reinterpret_cast<Node*>(index.internalPointer());
}

RemoteModel::Node* RemoteModel::nodeForIndex(const Protocol::ModelIndex &index) const
{
  Node *node = m_root;
  for (int i = 0; i < index.size(); ++i) {
    node = node->children[index[i].first];
  }
  return node;
}

QModelIndex RemoteModel::modelIndexForNode(Node* node, int column) const
{
  Q_ASSERT(node);
  if (node == m_root)
    return QModelIndex();
  return createIndex(node->parent->children.indexOf(node), column, node);
}

void RemoteModel::requestRowColumnCount(const QModelIndex &index) const
{
  Node *node = nodeForIndex(index);
  Q_ASSERT(node);
  Q_ASSERT(node->rowCount < 0 && node->columnCount < 0);

  if (node->rowCount < -1) // already requesting
    return;
  node->rowCount = -2;

  qDebug() << Q_FUNC_INFO << index << Protocol::fromQModelIndex(index);
  Message msg;
  msg.stream() << Protocol::ModelRowColumnCountRequest << Protocol::fromQModelIndex(index);
  Client::stream() << msg;
}

void RemoteModel::requestDataAndFlags(const QModelIndex& index) const
{
  Node *node = nodeForIndex(index);
  Q_ASSERT(node);
  Q_ASSERT(!node->data.contains(index.column()));
  Q_ASSERT(!node->flags.contains(index.column()));

  node->data.insert(index.column(), QHash<int, QVariant>()); // mark pending request
  qDebug() << Q_FUNC_INFO << index << Protocol::fromQModelIndex(index);

  Message msg;
  msg.stream() << Protocol::ModelContentRequest << Protocol::fromQModelIndex(index);
  Client::stream() << msg;
}

void RemoteModel::requestHeaderData(Qt::Orientation orientation, int section) const
{
  Q_ASSERT(!m_headers.value(orientation).contains(section));
  m_headers[orientation][section][Qt::DisplayRole] = tr("Loading...");

  Message msg;
  msg.stream() << Protocol::ModelHeaderRequest << qint8(orientation) << qint32(section);
  Client::stream() << msg;
}

#include "remotemodel.moc"
