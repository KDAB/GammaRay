/*
  metaobjecttreemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

using namespace GammaRay;

#define IF_DEBUG(x)

MetaObjectTreeModel::MetaObjectTreeModel(QObject* parent)
  : QAbstractItemModel(parent)
{
}

QVariant MetaObjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole) {
    switch (section) {
      case 0:
        return tr("Meta Object Hierachy");
      default:
        return QVariant();
    }
  }

  return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant MetaObjectTreeModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  const int column = index.column();
  const QMetaObject* object = metaObjectForIndex(index);
  if (role == Qt::DisplayRole) {
    switch(column) {
      case 0:
        return object->className();
      default:
        break;
    }
  }
  else if (role == MetaObjectRole) {
    return QVariant::fromValue<const QMetaObject*>(object);
  }
  return QVariant();
}

int MetaObjectTreeModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 1;
}

int MetaObjectTreeModel::rowCount(const QModelIndex& parent) const
{
  const QMetaObject* metaObject = metaObjectForIndex(parent);
  return m_parentChildMap.value(metaObject).size();
}

QModelIndex MetaObjectTreeModel::parent(const QModelIndex& child) const
{
  if (!child.isValid())
    return QModelIndex();

  const QMetaObject* object = metaObjectForIndex(child);
  Q_ASSERT(object);
  const QMetaObject* parentObject = object->superClass();
  return indexForMetaObject(parentObject);
}

QModelIndex MetaObjectTreeModel::index(int row, int column, const QModelIndex& parent) const
{
  const QMetaObject* parentObject = metaObjectForIndex(parent);
  const QVector<const QMetaObject*> children = m_parentChildMap.value(parentObject);
  if (row < 0 || column < 0 || row >= children.size()  || column >= columnCount()) {
    return QModelIndex();
  }

  return createIndex(row, column, (void*)children.at(row));
}

void MetaObjectTreeModel::objectAdded(QObject* obj)
{
  // slot, hence should always land in main thread due to auto connection
  Q_ASSERT(thread() == QThread::currentThread());

  ReadOrWriteLocker objectLock(Probe::instance()->objectLock());
  if (!Probe::instance()->isValidObject(obj)) {
    return;
  }
  Q_ASSERT(!obj->parent() || Probe::instance()->isValidObject(obj->parent()));

  const QMetaObject* metaObject = obj->metaObject();
  addMetaObject(metaObject);
}

void MetaObjectTreeModel::addMetaObject(const QMetaObject* metaObject)
{
  if (indexForMetaObject(metaObject).isValid()) {
    return;
  }

  const QMetaObject* parentMetaObject = metaObject->superClass();
  if (parentMetaObject) {
    const QModelIndex parentIndex = indexForMetaObject(parentMetaObject);
    if (!parentIndex.isValid()) {
      // add parent first
      addMetaObject(metaObject->superClass());
    }
  }

  const QModelIndex parentIndex = indexForMetaObject(parentMetaObject);
  // either we get a proper parent and hence valid index or there is no parent
  Q_ASSERT(parentIndex.isValid() || !parentMetaObject);

  QVector<const QMetaObject*> &children = m_parentChildMap[ parentMetaObject ];

  beginInsertRows(parentIndex, children.size(), children.size());
  children.push_back(metaObject);
  m_childParentMap.insert(metaObject, parentMetaObject);
  endInsertRows();
}

void MetaObjectTreeModel::removeMetaObject(const QMetaObject* metaObject)
{
  // TODO: Can this even happen?
}

void MetaObjectTreeModel::objectRemoved(QObject* obj)
{
  // TODO
}

QModelIndex MetaObjectTreeModel::indexForMetaObject(const QMetaObject* metaObject) const
{
  if (!metaObject)
    return QModelIndex();

  const QMetaObject *parentObject = m_childParentMap.value(metaObject);
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

const QMetaObject* MetaObjectTreeModel::metaObjectForIndex(const QModelIndex& index) const
{
  if (!index.isValid())
    return 0;

  void* internalPointer = index.internalPointer();
  const QMetaObject* metaObject = reinterpret_cast<QMetaObject*>(internalPointer);
  return metaObject;
}

#include "metaobjecttreemodel.moc"
