#include "quickitemmodel.h"

#include <QDebug>
#include <QQuickItem>
#include <QQuickWindow>
#include <QThread>

#include <algorithm>

using namespace GammaRay;

QuickItemModel::QuickItemModel(QObject* parent) : ObjectModelBase<QAbstractItemModel>(parent)
{
}

QuickItemModel::~QuickItemModel()
{
}

void QuickItemModel::setWindow(QQuickWindow* window)
{
  beginResetModel();
  clear();
  populateFromItem(window->contentItem());
  endResetModel();
}

QVariant QuickItemModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  QQuickItem *item = reinterpret_cast<QQuickItem*>(index.internalPointer());
  return dataForObject(item, index, role);
}

int QuickItemModel::rowCount(const QModelIndex& parent) const
{
  if (parent.column() == 1) {
    return 0;
  }
  QQuickItem *parentItem = reinterpret_cast<QQuickItem*>(parent.internalPointer());
  return m_parentChildMap.value(parentItem).size();
}

QModelIndex QuickItemModel::parent(const QModelIndex& child) const
{
  QQuickItem *childItem = reinterpret_cast<QQuickItem*>(child.internalPointer());
  return indexForItem(m_childParentMap.value(childItem));
}

QModelIndex QuickItemModel::index(int row, int column, const QModelIndex& parent) const
{
  QQuickItem *parentItem = reinterpret_cast<QQuickItem*>(parent.internalPointer());
  const QVector<QQuickItem*> children = m_parentChildMap.value(parentItem);
  if (row < 0 || column < 0 || row >= children.size()  || column >= columnCount()) {
    return QModelIndex();
  }
  return createIndex(row, column, children.at(row));
}

void QuickItemModel::clear()
{
  m_childParentMap.clear();
  m_parentChildMap.clear();
}

void QuickItemModel::populateFromItem(QQuickItem* item)
{
  if (!item)
    return;

  m_childParentMap[item] = item->parentItem();
  m_parentChildMap[item->parentItem()].push_back(item);

  foreach(QQuickItem *child, item->childItems())
    populateFromItem(child);

  QVector<QQuickItem*> &children  = m_parentChildMap[item->parentItem()];
  std::sort(children.begin(), children.end());
}

QModelIndex QuickItemModel::indexForItem(QQuickItem* item) const
{
  if (!item)
    return QModelIndex();

  QQuickItem *parent = m_childParentMap.value(item);
  const QModelIndex parentIndex = indexForItem(parent);
  if (!parentIndex.isValid() && parent) {
    return QModelIndex();
  }
  const QVector<QQuickItem*> &siblings = m_parentChildMap[parent];
  QVector<QQuickItem*>::const_iterator it = std::lower_bound(siblings.constBegin(), siblings.constEnd(), item);
  if (it == siblings.constEnd() || *it != item) {
    return QModelIndex();
  }

  const int row = std::distance(siblings.constBegin(), it);
  return index(row, 0, parentIndex);
}

void QuickItemModel::objectRemoved(QObject* obj)
{
  Q_ASSERT(thread() == QThread::currentThread());
  QQuickItem *item = static_cast<QQuickItem*>(obj); // this is fine, we must not dereference obj/item at this point anyway

  if (!m_childParentMap.contains(item)) { // not an item of our current scene
    Q_ASSERT(!m_parentChildMap.contains(item));
    return;
  }

  QQuickItem *parentItem = m_childParentMap[item];
  const QModelIndex parentIndex = indexForItem(parentItem);
  if (parentItem && !parentIndex.isValid()) {
    return;
  }

  QVector<QQuickItem*> &siblings = m_parentChildMap[parentItem];
  QVector<QQuickItem*>::iterator it = std::lower_bound(siblings.begin(), siblings.end(), item);
  if (it == siblings.end() || *it != item) {
    return;
  }
  const int row = std::distance(siblings.begin(), it);

  beginRemoveRows(parentIndex, row, row);

  siblings.erase(it);
  m_childParentMap.remove(item);
  m_parentChildMap.remove(item);

  endRemoveRows();
}
