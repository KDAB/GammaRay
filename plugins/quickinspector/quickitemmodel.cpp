/*
  quickitemmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "quickitemmodel.h"
#include "quickitemmodelroles.h"

#include <QQuickItem>
#include <QQuickWindow>
#include <QThread>
#include <QQmlEngine>
#include <QQmlContext>
#include <QEvent>

#include <algorithm>

using namespace GammaRay;

QuickItemModel::QuickItemModel(QObject *parent) : ObjectModelBase<QAbstractItemModel>(parent)
{
}

QuickItemModel::~QuickItemModel()
{
}

void QuickItemModel::setWindow(QQuickWindow *window)
{
  beginResetModel();
  clear();
  m_window = window;
  populateFromItem(window->contentItem());
  endResetModel();
}

QVariant QuickItemModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  QQuickItem *item = reinterpret_cast<QQuickItem*>(index.internalPointer());

  if (role == QuickItemModelRole::ItemFlags) {
    return m_itemFlags[item];
  }
  if (role == Qt::DisplayRole && index.column() == 0) {
    QQmlContext *ctx = QQmlEngine::contextForObject(item);
    QString id = ctx ? ctx->nameForObject(item) : "";
    if (!id.isEmpty()) {
      return id;
    }
  }

  return dataForObject(item, index, role);
}

int QuickItemModel::rowCount(const QModelIndex &parent) const
{
  if (parent.column() == 1) {
    return 0;
  }

  QQuickItem *parentItem = reinterpret_cast<QQuickItem*>(parent.internalPointer());

  return m_parentChildMap.value(parentItem).size();
}

QModelIndex QuickItemModel::parent(const QModelIndex &child) const
{
  QQuickItem *childItem = reinterpret_cast<QQuickItem*>(child.internalPointer());
  return indexForItem(m_childParentMap.value(childItem));
}

QModelIndex QuickItemModel::index(int row, int column, const QModelIndex &parent) const
{
  QQuickItem *parentItem = reinterpret_cast<QQuickItem*>(parent.internalPointer());
  const QVector<QQuickItem*> children = m_parentChildMap.value(parentItem);
  if (row < 0 || column < 0 || row >= children.size()  || column >= columnCount()) {
    return QModelIndex();
  }
  return createIndex(row, column, children.at(row));
}

QMap<int, QVariant> QuickItemModel::itemData(const QModelIndex &index) const
{
  QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
  d.insert(QuickItemModelRole::ItemFlags, data(index, QuickItemModelRole::ItemFlags));
  return d;
}

void QuickItemModel::clear()
{
  for (QHash<QQuickItem*, QQuickItem*>::const_iterator it = m_childParentMap.constBegin();
       it != m_childParentMap.constEnd(); ++it) {
    disconnect(it.key(), 0, this, 0);
  }
  m_childParentMap.clear();
  m_parentChildMap.clear();
}

void QuickItemModel::populateFromItem(QQuickItem *item)
{
  if (!item) {
    return;
  }

  connectItem(item);
  updateItemFlags(item);
  m_childParentMap[item] = item->parentItem();
  m_parentChildMap[item->parentItem()].push_back(item);

  foreach (QQuickItem *child, item->childItems()) {
    populateFromItem(child);
  }

  QVector<QQuickItem*> &children  = m_parentChildMap[item->parentItem()];
  std::sort(children.begin(), children.end());
}

void QuickItemModel::connectItem(QQuickItem *item)
{
  connect(item, SIGNAL(parentChanged(QQuickItem*)), this, SLOT(itemReparented()));
  connect(item, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(itemWindowChanged()));
  connect(item, SIGNAL(visibleChanged()), this, SLOT(itemUpdated()));
  connect(item, SIGNAL(focusChanged(bool)), this, SLOT(itemUpdated()));
  connect(item, SIGNAL(activeFocusChanged(bool)), this, SLOT(itemUpdated()));
  connect(item, SIGNAL(widthChanged()), this, SLOT(itemUpdated()));
  connect(item, SIGNAL(heightChanged()), this, SLOT(itemUpdated()));
  connect(item, SIGNAL(xChanged()), this, SLOT(itemUpdated()));
  connect(item, SIGNAL(yChanged()), this, SLOT(itemUpdated()));
  item->installEventFilter(new QuickEventMonitor(this));
}

void QuickItemModel::disconnectItem(QQuickItem *item)
{
  disconnect(item, 0, this, 0);
}

QModelIndex QuickItemModel::indexForItem(QQuickItem *item) const
{
  if (!item) {
    return QModelIndex();
  }

  QQuickItem *parent = m_childParentMap.value(item);
  const QModelIndex parentIndex = indexForItem(parent);
  if (!parentIndex.isValid() && parent) {
    return QModelIndex();
  }

  const QVector<QQuickItem*> &siblings = m_parentChildMap[parent];
  QVector<QQuickItem*>::const_iterator it =
    std::lower_bound(siblings.constBegin(), siblings.constEnd(), item);
  if (it == siblings.constEnd() || *it != item) {
    return QModelIndex();
  }

  const int row = std::distance(siblings.constBegin(), it);
  return index(row, 0, parentIndex);
}

void QuickItemModel::objectAdded(QObject *obj)
{
  Q_ASSERT(thread() == QThread::currentThread());
  QQuickItem *item = qobject_cast<QQuickItem*>(obj);
  addItem(item);
}

void QuickItemModel::addItem(QQuickItem *item)
{
  if (!item) {
    return;
  }

  if (!item->window()) {
    return; // item not (yet) added to a scene
  }

  if (item->window() != m_window) {
    return; // item for a different scene
  }

  if (m_childParentMap.contains(item)) {
    return; // already known
  }

  QQuickItem *parentItem = item->parentItem();
  if (parentItem) {
    // add parent first, if we don't know that yet
    if (!m_childParentMap.contains(parentItem)) {
      objectAdded(parentItem);
    }
  }

  connectItem(item);

  const QModelIndex index = indexForItem(parentItem);
  Q_ASSERT(index.isValid() || !parentItem);

  QVector<QQuickItem*> &children = m_parentChildMap[parentItem];
  QVector<QQuickItem*>::iterator it = std::lower_bound(children.begin(), children.end(), item);
  const int row = std::distance(children.begin(), it);

  beginInsertRows(index, row, row);
  children.insert(it, item);
  m_childParentMap.insert(item, parentItem);
  endInsertRows();
}

void QuickItemModel::objectRemoved(QObject *obj)
{
  Q_ASSERT(thread() == QThread::currentThread());
  QQuickItem *item = static_cast<QQuickItem*>(obj); // this is fine, we must not deref
                                                    // obj/item at this point anyway
  removeItem(item, true);
}

void QuickItemModel::removeItem(QQuickItem *item, bool danglingPointer)
{
  if (!m_childParentMap.contains(item)) { // not an item of our current scene
    Q_ASSERT(!m_parentChildMap.contains(item));
    return;
  }

  if (item && !danglingPointer) {
    disconnectItem(item);
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
  doRemoveSubtree(item, danglingPointer);

  endRemoveRows();
}

void QuickItemModel::doRemoveSubtree(QQuickItem *item, bool danglingPointer)
{
  m_childParentMap.remove(item);
  m_parentChildMap.remove(item);
  if (!danglingPointer) {
    foreach (QQuickItem *child, item->childItems()) {
      doRemoveSubtree(child, false);
    }
  }
}

void QuickItemModel::itemReparented()
{
  QQuickItem *item = qobject_cast<QQuickItem*>(sender());
  if (!item->parentItem()) { // Item was not deleted, but removed from the scene.
    removeItem(item, false);
    return;
  }

  Q_ASSERT(item && item->window() == m_window);

  QQuickItem *sourceParent = m_childParentMap.value(item);
  Q_ASSERT(sourceParent);
  const QModelIndex sourceParentIndex = indexForItem(sourceParent);

  QVector<QQuickItem*> &sourceSiblings = m_parentChildMap[sourceParent];
  QVector<QQuickItem*>::iterator sit =
    std::lower_bound(sourceSiblings.begin(), sourceSiblings.end(), item);
  Q_ASSERT(sit != sourceSiblings.end() && *sit == item);
  const int sourceRow = std::distance(sourceSiblings.begin(), sit);

  QQuickItem *destParent = item->parentItem();
  Q_ASSERT(destParent);
  const QModelIndex destParentIndex = indexForItem(destParent);

  QVector<QQuickItem*> &destSiblings = m_parentChildMap[destParent];
  QVector<QQuickItem*>::iterator dit =
    std::lower_bound(destSiblings.begin(), destSiblings.end(), item);
  const int destRow = std::distance(destSiblings.begin(), dit);

  beginMoveRows(sourceParentIndex, sourceRow, sourceRow, destParentIndex, destRow);
  destSiblings.insert(dit, item);
  sourceSiblings.erase(sit);
  m_childParentMap.insert(item, destParent);
  endMoveRows();
}

void QuickItemModel::itemWindowChanged()
{
  QQuickItem *item = qobject_cast<QQuickItem*>(sender());
  Q_ASSERT(item && (!item->window() || item->window() != m_window));
  removeItem(item);
}

void QuickItemModel::itemUpdated()
{
  QQuickItem *item = qobject_cast<QQuickItem*>(sender());

  recursivelyUpdateItem(item);
}

void QuickItemModel::recursivelyUpdateItem(QQuickItem *item)
{
  int oldFlags = m_itemFlags[item];
  updateItemFlags(item);

  if (oldFlags != m_itemFlags[item]) {
    updateItem(item);
  }

  foreach (QQuickItem *child, item->childItems()) {
    recursivelyUpdateItem(child);
  }
}

void QuickItemModel::updateItem(QQuickItem *item)
{
  if (!item || item->window() != m_window) {
    return;
  }

  const QModelIndex left = indexForItem(item);
  const QModelIndex right = left.sibling(left.row(), columnCount() - 1);
  emit dataChanged(left, right);
}

void QuickItemModel::updateItemFlags(QQuickItem *item)
{
  QQuickItem *ancestor = item->parentItem();
  bool outOfView = false;
  while (ancestor && ancestor != m_window->contentItem()) {
    QPointF pos = ancestor->mapFromItem(item, QPointF(0, 0));
    if ((ancestor->parentItem() == m_window->contentItem() ||
         ancestor->clip()) &&
        (-pos.x() > item->width() || -pos.y() > item->height() ||
         pos.x() > ancestor->width() || pos.y() > ancestor->height())) {
      outOfView = true;
      break;
    }
    ancestor = ancestor->parentItem();
  }
  m_itemFlags[item] = (!item->isVisible() || item->opacity() == 0 ?
                        QuickItemModelRole::Invisible : QuickItemModelRole::None) |
                      (item->width() == 0 || item->height() == 0 ?
                        QuickItemModelRole::ZeroSize : QuickItemModelRole::None) |
                      (outOfView ?
                        QuickItemModelRole::OutOfView : QuickItemModelRole::None) |
                      (item->hasFocus() ?
                        QuickItemModelRole::HasFocus : QuickItemModelRole::None) |
                      (item->hasActiveFocus() ?
                        QuickItemModelRole::HasActiveFocus : QuickItemModelRole::None);
}

QuickEventMonitor::QuickEventMonitor(QuickItemModel *parent)
  : QObject(parent), m_model(parent)
{
}

bool QuickEventMonitor::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() != QEvent::DeferredDelete && event->type() != QEvent::Destroy) {
    // exclude some unsafe event types
    m_model->updateItem(qobject_cast<QQuickItem*>(obj));
  }

  return false;
}
