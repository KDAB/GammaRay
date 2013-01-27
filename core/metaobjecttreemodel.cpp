/*
  metaobjecttreemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "metaobjecttreemodel.h"

#include "probe.h"
#include "readorwritelocker.h"

#include <QDebug>
#include <QThread>

#include <assert.h>

using namespace GammaRay;

#define IF_DEBUG(x)

namespace GammaRay {

/**
 * Tracks information about meta objects
 *
 * @sa objectAdded for explanation
 */
class MetaObjectInfoTracker
{
public:
  struct MetaObjectInfo
  {
    MetaObjectInfo() : selfCount(0), inclusiveCount(0) {}

    /// Number of objects of a particular meta object type
    int selfCount;
    /**
     * Number of objects of the exact meta object type
     * + number of objects of type that inherit from this meta type
     */
    int inclusiveCount;
  };

  /**
   * Use this whenever a new object of type @p metaObject was seen
   *
   * This will increase these values:
   * - selfCount for that particular @p metaObject
   * - inclusiveCount for @p metaObject and *all* ancestors
   *
   * Complexity-wise the inclusive count calculation should be okay,
   * since the number of ancestors should be rather small
   * (QMetaObject class hierarchy is rather a broad than a deep tree structure)
   *
   * If this yields some performance issues, we might need to remove the inclusive
   * costs calculation altogether (a calculate-on-request pattern should be even slower)
   */
  void objectAdded(const QMetaObject* metaObject)
  {
    // note: use plain C asserts here, infinite loops otherwise in case of assert

    ++m_metaObjectInfoMap[metaObject].selfCount;

    // increase inclusive counts
    const QMetaObject* current = metaObject;
    while (current) {
      ++m_metaObjectInfoMap[current].inclusiveCount;
      current = current->superClass();
    }
  }

  /**
   * @sa objectAdded for explanation
   */
  void objectRemoved(const QMetaObject* metaObject)
  {
    // note: use plain C asserts here, infinite loops otherwise

    assert(m_metaObjectInfoMap.contains(metaObject));
    if (m_metaObjectInfoMap[metaObject].selfCount == 0) {
      // something went wrong, but let's just ignore this event in case of assert
      return;
    }

    --m_metaObjectInfoMap[metaObject].selfCount;
    assert(m_metaObjectInfoMap[metaObject].selfCount >= 0);

    // decrease inclusive counts
    const QMetaObject* current = metaObject;
    while (current) {
      --m_metaObjectInfoMap[current].inclusiveCount;
      assert(m_metaObjectInfoMap[current].inclusiveCount >= 0);
      current = current->superClass();
    }
  }

  inline const MetaObjectInfo& info(const QMetaObject* metaObject)
  {
    return m_metaObjectInfoMap[metaObject];
  }

private:
  QHash<const QMetaObject*, MetaObjectInfo> m_metaObjectInfoMap;
};

}

MetaObjectTreeModel::MetaObjectTreeModel(QObject *parent)
  : QAbstractItemModel(parent)
  , m_infoTracker(new MetaObjectInfoTracker)
{
  qRegisterMetaType<const QMetaObject *>();
}

MetaObjectTreeModel::~MetaObjectTreeModel()
{
  delete m_infoTracker;
}

QVariant MetaObjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    switch (section) {
      case ObjectColumn:
        return tr("Meta Object Class");
      case ObjectSelfCountColumn:
        return tr("Self (#Objects)");
      case ObjectInclusiveCountColumn:
        return tr("Incl. (#Objects)");
      default:
        return QVariant();
    }
  } else if (role == Qt::ToolTipRole) {
    switch (section) {
    case ObjectColumn:
      return tr("This column shows the QMetaObject class hierarchy");
    case ObjectSelfCountColumn:
      return tr("This column shows the number of objects of particular type");
    case ObjectInclusiveCountColumn:
      return tr("This column shows the number of objects that inherit from a particular type");
    default:
      return QVariant();
    }
  }

  return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant MetaObjectTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  const int column = index.column();
  const QMetaObject *object = metaObjectForIndex(index);
  if (role == Qt::DisplayRole) {
    switch(column) {
    case ObjectColumn:
      return object->className();
      case ObjectSelfCountColumn:
        return m_infoTracker->info(object).selfCount;
      case ObjectInclusiveCountColumn:
        return m_infoTracker->info(object).inclusiveCount;
    default:
      break;
    }
  } else if (role == MetaObjectRole) {
    return QVariant::fromValue<const QMetaObject*>(object);
  }
  return QVariant();
}

int MetaObjectTreeModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return _Last;
}

int MetaObjectTreeModel::rowCount(const QModelIndex &parent) const
{
  const QMetaObject *metaObject = metaObjectForIndex(parent);
  return m_parentChildMap.value(metaObject).size();
}

QModelIndex MetaObjectTreeModel::parent(const QModelIndex &child) const
{
  if (!child.isValid()) {
    return QModelIndex();
  }

  const QMetaObject *object = metaObjectForIndex(child);
  Q_ASSERT(object);
  const QMetaObject *parentObject = object->superClass();
  return indexForMetaObject(parentObject);
}

QModelIndex MetaObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  const QMetaObject *parentObject = metaObjectForIndex(parent);
  const QVector<const QMetaObject*> children = m_parentChildMap.value(parentObject);
  if (row < 0 || column < 0 || row >= children.size()  || column >= columnCount()) {
    return QModelIndex();
  }

  const QMetaObject *object = children.at(row);
  return createIndex(row, column, const_cast<QMetaObject*>(object));
}

void MetaObjectTreeModel::objectAdded(QObject *obj)
{
  // when called from background, delay into fore&ground, otherwise call directly
  QMetaObject::invokeMethod(this, "objectAddedMainThread", Qt::AutoConnection,
                            Q_ARG(QObject *, obj));
}

void MetaObjectTreeModel::objectAddedMainThread(QObject* obj)
{
  // slot, hence should always land in main thread due to auto connection
  assert(thread() == QThread::currentThread());

  ReadOrWriteLocker objectLock(Probe::instance()->objectLock());
  if (!Probe::instance()->isValidObject(obj)) {
    return;
  }
  assert(!obj->parent() || Probe::instance()->isValidObject(obj->parent()));

  QWriteLocker lock(&m_lock);
  const QMetaObject *metaObject = obj->metaObject();
  addMetaObject(metaObject);

  // increase counter
  const QModelIndex metaModelIndex = indexForMetaObject(metaObject);
  assert(metaModelIndex.isValid());
  m_infoTracker->objectAdded(metaObject);
  emit dataChanged(metaModelIndex, metaModelIndex);
}

void MetaObjectTreeModel::addMetaObject(const QMetaObject *metaObject)
{
  if (indexForMetaObject(metaObject).isValid()) {
    return;
  }

  const QMetaObject *parentMetaObject = metaObject->superClass();
  if (parentMetaObject) {
    const QModelIndex parentIndex = indexForMetaObject(parentMetaObject);
    if (!parentIndex.isValid()) {
      // add parent first
      addMetaObject(metaObject->superClass());
    }
  }

  const QModelIndex parentIndex = indexForMetaObject(parentMetaObject);
  // either we get a proper parent and hence valid index or there is no parent
  assert(parentIndex.isValid() || !parentMetaObject);

  QVector<const QMetaObject*> &children = m_parentChildMap[ parentMetaObject ];

  beginInsertRows(parentIndex, children.size(), children.size());
  children.push_back(metaObject);
  m_childParentMap.insert(metaObject, parentMetaObject);
  endInsertRows();
}

void MetaObjectTreeModel::removeMetaObject(const QMetaObject *metaObject)
{
  Q_UNUSED(metaObject);
  // TODO: Can this even happen?
}

void MetaObjectTreeModel::objectRemoved(QObject *obj)
{
  // we need to find out the meta object *now*
  // in objectRemovedMainThread the QObject* is no longer valid
  const QMetaObject* metaObject = obj->metaObject();

  // when called from background, delay into foreground, otherwise call directly
  QMetaObject::invokeMethod(this, "objectRemovedMainThread", Qt::AutoConnection,
                            Q_ARG(const QMetaObject *, metaObject));
}

// We're just interested in the meta object of the object that was removed
// Hence the QMetaObject* parameter
void MetaObjectTreeModel::objectRemovedMainThread(const QMetaObject *metaObject)
{
  assert(thread() == QThread::currentThread());

  QWriteLocker lock(&m_lock);

    // decrease counter
  const QModelIndex metaModelIndex = indexForMetaObject(metaObject);
  if (!metaModelIndex.isValid()) {
    // something went wrong, ignore
    return;
  }

  m_infoTracker->objectRemoved(metaObject);
  emit dataChanged(metaModelIndex, metaModelIndex);
}

QModelIndex MetaObjectTreeModel::indexForMetaObject(const QMetaObject *metaObject) const
{
  if (!metaObject) {
    return QModelIndex();
  }

  const QMetaObject *parentObject = m_childParentMap.value(metaObject);
  Q_ASSERT(parentObject != metaObject);
  const QModelIndex parentIndex = indexForMetaObject(parentObject);
  if (!parentIndex.isValid() && parentObject) {
    return QModelIndex();
  }

  const int row = m_parentChildMap[parentObject].indexOf(metaObject);
  if (row < 0) {
    return QModelIndex();
  }

  return index(row, 0, parentIndex);
}

const QMetaObject *MetaObjectTreeModel::metaObjectForIndex(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return 0;
  }

  void *internalPointer = index.internalPointer();
  const QMetaObject* metaObject = reinterpret_cast<QMetaObject*>(internalPointer);
  return metaObject;
}

#include "metaobjecttreemodel.moc"
