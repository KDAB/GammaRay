/*
  quickscenegraphmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "quickscenegraphmodel.h"

#ifdef HAVE_PRIVATE_QT_HEADERS

#include <private/qquickitem_p.h> //krazy:exclude=camelcase
#include "quickitemmodelroles.h"

#include <QDebug>
#include <QQuickWindow>
#include <QThread>
#include <QSGNode>

#include <algorithm>

Q_DECLARE_METATYPE(QSGNode*)

using namespace GammaRay;

QuickSceneGraphModel::QuickSceneGraphModel(QObject* parent) : ObjectModelBase<QAbstractItemModel>(parent)
{
}

QuickSceneGraphModel::~QuickSceneGraphModel()
{
}

void QuickSceneGraphModel::setWindow(QQuickWindow* window)
{
  beginResetModel();
  clear();
  m_window = window;
  QQuickItem *item = window->contentItem();
  QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
  m_rootNode = itemPriv->itemNode();
  populateFromNode(m_rootNode);
  collectItemNodes(item);

  printf(QString::number(m_itemItemNodeMap.size()).append("   <---  Size of itemItemNodeMap.\n").toAscii());

  endResetModel();
}

QVariant QuickSceneGraphModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  QSGNode *node = reinterpret_cast<QSGNode*>(index.internalPointer());
  node = node ? node : m_rootNode;


  if (role == Qt::DisplayRole) {
    if (index.column() == 0) {
      return Util::addressToString(node);
    } else if (index.column() == 1) {
      switch (node->type()) {
      case QSGNode::BasicNodeType:
        return "Node";
      case QSGNode::GeometryNodeType:
        return "Geometry Node";
      case QSGNode::TransformNodeType:
        return "Transform Node";
      case QSGNode::ClipNodeType:
        return "Clip Node";
      case QSGNode::OpacityNodeType:
        return "Opacity Node";
      case QSGNode::RootNodeType:
        return "Root Node";
      case QSGNode::RenderNodeType:
        return "Render Node";
      }
    }
  } else if (role == ObjectModel::ObjectRole) {
    return QVariant::fromValue(node);
  }

  return QVariant();
}

int QuickSceneGraphModel::rowCount(const QModelIndex& parent) const
{
  if (parent.column() == 1) {
    return 0;
  }
  QSGNode *parentNode = reinterpret_cast<QSGNode*>(parent.internalPointer());
  parentNode = parentNode ? parentNode : m_rootNode;
  return m_parentChildMap.value(parentNode).size();
}

QModelIndex QuickSceneGraphModel::parent(const QModelIndex& child) const
{
  QSGNode *childNode = reinterpret_cast<QSGNode*>(child.internalPointer());
  return indexForNode(m_childParentMap.value(childNode));
}

QModelIndex QuickSceneGraphModel::index(int row, int column, const QModelIndex& parent) const
{
  QSGNode *parentNode = reinterpret_cast<QSGNode*>(parent.internalPointer());
  parentNode = parentNode ? parentNode : m_rootNode;
  const QVector<QSGNode*> children = m_parentChildMap.value(parentNode);
  if (row < 0 || column < 0 || row >= children.size()  || column >= columnCount()) {
    return QModelIndex();
  }
  return createIndex(row, column, children.at(row));
}

//QMap<int, QVariant> QuickSceneGraphModel::itemData(const QModelIndex& index) const
//{
//  QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
//  d.insert(QuickItemModelRole::Visibility, data(index, QuickItemModelRole::Visibility));
//  return d;
//}

void QuickSceneGraphModel::clear()
{
  for (QHash<QSGNode*, QSGNode*>::const_iterator it = m_childParentMap.constBegin(); it != m_childParentMap.constEnd(); ++it)
//    disconnect(it.key(), 0, this, 0);
  m_childParentMap.clear();
  m_parentChildMap.clear();
}

//void QuickSceneGraphModel::populateFromItem(QQuickItem *item)
//{
//  if (!item)
//    return;

//  QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
//  QSGNode *node = itemPriv->paintNode;

////  connectNode(node);
//  m_childParentMap[item] = item->parentItem();
//  m_parentChildMap[item->parentItem()].push_back(item);

//  foreach(QQuickItem *child, item->childItems())
//    populateFromItem(child);

//  for (int i = 0; i < node->childCount(); i++)
//    populateFromNode(node->childAtIndex(i), item);

//  QVector<QSGNode*> &children  = m_parentChildMap[item->parentItem()];
//  std::sort(children.begin(), children.end());
//}

void QuickSceneGraphModel::populateFromNode(QSGNode *node)
{
  if (!node)
    return;

//  connectNode(node);
  m_childParentMap[node] = node->parent();
  m_parentChildMap[node->parent()].push_back(node);

  for (int i = 0; i < node->childCount(); i++) {
    populateFromNode(node->childAtIndex(i));
  }

  QVector<QSGNode*> &children  = m_parentChildMap[node->parent()];
  std::sort(children.begin(), children.end());
}

void QuickSceneGraphModel::collectItemNodes(QQuickItem *item)
{
    if (!item)
        return;
    m_itemItemNodeMap.insert(item, QQuickItemPrivate::get(item)->itemNode());

    foreach (QQuickItem *child, item->childItems())
        collectItemNodes(child);
}

void QuickSceneGraphModel::connectNode(QSGNode* item)
{
//  connect(item, SIGNAL(parentChanged(QSGNode*)), this, SLOT(itemReparented()));
//  connect(item, SIGNAL(visibleChanged()), this, SLOT(itemUpdated()));
}


QModelIndex QuickSceneGraphModel::indexForNode(QSGNode* node) const
{
  if (!node)
    return QModelIndex();

  QSGNode *parent = m_childParentMap.value(node);
  const QModelIndex parentIndex = indexForNode(parent);
  if (!parentIndex.isValid() && parent) {
    return QModelIndex();
  }
  const QVector<QSGNode*> &siblings = m_parentChildMap[parent];
  QVector<QSGNode*>::const_iterator it = std::lower_bound(siblings.constBegin(), siblings.constEnd(), node);
  if (it == siblings.constEnd() || *it != node) {
    return QModelIndex();
  }

  const int row = std::distance(siblings.constBegin(), it);
  return index(row, 0, parentIndex);
}

void QuickSceneGraphModel::objectAdded(QObject* obj)
{
  Q_ASSERT(thread() == QThread::currentThread());
  QQuickItem *item = qobject_cast<QQuickItem*>(obj);
  if (!item)
    return;

  if (item->window() != m_window)
    return; // item for a different scene

  printf("\nHHHHHUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU!!! ");
  printf(item->parentItem()->parentItem()->objectName().toAscii());
  addNode(QQuickItemPrivate::get(item)->itemNode());
}

void QuickSceneGraphModel::addNode(QSGNode *node)
{
    if (!node)
      return;

    if (m_childParentMap.contains(node))
      return; // already known


    QSGNode *parentNode = node->parent();
    if (parentNode) {
        printf("Parent does exist.");
      // add parent first, if we don't know that yet
      if (!m_childParentMap.contains(parentNode))
        addNode(parentNode);
    } else
        printf("Parent does NOT exist.");

  //  connectNode(item);

    const QModelIndex index = indexForNode(parentNode);
    Q_ASSERT(index.isValid() || !parentNode);

    QVector<QSGNode*> &children = m_parentChildMap[parentNode];
    QVector<QSGNode*>::iterator it = std::lower_bound(children.begin(), children.end(), node);
    const int row = std::distance(children.begin(), it);

    beginInsertRows(index, row, row);
    children.insert(it, node);
    m_childParentMap.insert(node, parentNode);
    endInsertRows();
}

void QuickSceneGraphModel::objectRemoved(QObject* obj)
{
  Q_ASSERT(thread() == QThread::currentThread());
  QQuickItem *item = static_cast<QQuickItem*>(obj); // this is fine, we must not dereference obj/item at this point anyway
  printf("\nIIIIIIIIIIIIIIIIIIIIIHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH!!!!!!!!! ");
  printf(QString::number(m_itemItemNodeMap.size()).toAscii());

  if (!item || !m_itemItemNodeMap.contains(item)) { // not an item of our current scene
    printf("dfhgdhgfölaskdfjslakdfjsölkdfjöslkjfsalkjflsakdjfsaölkdfslkdföaslkdfjösalkfjaösldkfj Not in itemItemNodeMap.\n");
    QString mapStr;
    foreach (QQuickItem *key, m_itemItemNodeMap.keys())
        mapStr.append(" ").append(QString::number((qlonglong)key));
    printf("\n");
    printf(mapStr.toAscii());
    printf(mapStr.contains(QString::number((qlonglong)item)) ? "CONTAINS" : "Does not contain.");
    printf("\n");
    return;
  }
  printf("!!!!!!!!!!!!!IN itemItemNodeMap!!!!!!!!!!!!!!!1\n");

  removeNode(m_itemItemNodeMap[item]);
}

void QuickSceneGraphModel::removeNode(QSGNode *node)
{
  Q_ASSERT(thread() == QThread::currentThread());

  if (!m_childParentMap.contains(node)) { // not an item of our current scene
    Q_ASSERT(!m_parentChildMap.contains(node));
      printf("asdfölkjfölaskdfjslakdfjsölkdfjöslkjfsalkjflsakdjfsaölkdfslkdföaslkdfjösalkfjaösldkfj Not in childParentMap.\n");
    return;
  }

  foreach (QSGNode *childNode, m_parentChildMap[node])
      removeNode(childNode);

  QSGNode *parentNode = m_childParentMap[node];
  const QModelIndex parentIndex = indexForNode(parentNode);
  if (parentNode && !parentIndex.isValid()) {
    return;
  }

  QVector<QSGNode*> &siblings = m_parentChildMap[parentNode];
  QVector<QSGNode*>::iterator it = std::lower_bound(siblings.begin(), siblings.end(), node);
  if (it == siblings.end() || *it != node) {
    return;
  }
  const int row = std::distance(siblings.begin(), it);

  beginRemoveRows(parentIndex, row, row);

  siblings.erase(it);
  m_childParentMap.remove(node);
  m_parentChildMap.remove(node);

  endRemoveRows();
}

//void QuickSceneGraphModel::itemReparented()
//{
//  QQuickItem *item = qobject_cast<QQuickItem*>(sender());
//  if (!item || item->window() != m_window)
//    return;

//  QQuickItem* sourceParent = m_childParentMap.value(item);
//  Q_ASSERT(sourceParent);
//  const QModelIndex sourceParentIndex = indexForNode(sourceParent);

//  QVector<QQuickItem*> &sourceSiblings = m_parentChildMap[sourceParent];
//  QVector<QQuickItem*>::iterator sit = std::lower_bound(sourceSiblings.begin(), sourceSiblings.end(), item);
//  Q_ASSERT(sit != sourceSiblings.end() && *sit == item);
//  const int sourceRow = std::distance(sourceSiblings.begin(), sit);

//  QQuickItem* destParent = item->parentItem();
//  Q_ASSERT(destParent);
//  const QModelIndex destParentIndex = indexForNode(destParent);

//  QVector<QQuickItem*> &destSiblings = m_parentChildMap[destParent];
//  QVector<QQuickItem*>::iterator dit = std::lower_bound(destSiblings.begin(), destSiblings.end(), item);
//  const int destRow = std::distance(destSiblings.begin(), dit);

//  beginMoveRows(sourceParentIndex, sourceRow, sourceRow, destParentIndex, destRow);
//  destSiblings.insert(dit, item);
//  sourceSiblings.erase(sit);
//  m_childParentMap.insert(item, destParent);
//  endMoveRows();
//}

//void QuickSceneGraphModel::itemUpdated()
//{
//  QQuickItem *item = qobject_cast<QQuickItem*>(sender());
//  if (!item || item->window() != m_window)
//    return;

//  const QModelIndex left = indexForNode(item);
//  const QModelIndex right = left.sibling(left.row(), columnCount() - 1);
//  emit dataChanged(left, right);
//}

#endif // HAVE_PRIVATE_QT_HEADERS
