/*
  objecttreemodel.cpp

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objecttreemodel.h"

#include <QtCore/QEvent>

#include "readorwritelocker.h"

using namespace Endoscope;

ObjectTreeModel::ObjectTreeModel(QObject *parent)
  : ObjectModelBase< QAbstractItemModel >(parent)
  , m_lock(QReadWriteLock::Recursive)
{
}

void ObjectTreeModel::objectAdded(QObject *obj)
{
  QWriteLocker lock(&m_lock);
  if (!obj || m_childParentMap.contains(obj)) {
    return;
  }
  QVector<QObject*> &children = m_parentChildMap[ obj->parent() ];
  const QModelIndex index = indexForObject(obj->parent());
  if (index.isValid() || !obj->parent()) {
    beginInsertRows(index, children.size(), children.size());
  }
  children.push_back(obj);
  m_childParentMap.insert(obj, obj->parent());
  if (index.isValid() || !obj->parent()) {
    endInsertRows();
  }
}

void ObjectTreeModel::objectRemoved(QObject *obj)
{
  QWriteLocker lock(&m_lock);
  if (!m_childParentMap.contains(obj)) {
    return;
  }
  QObject *parentObj = m_childParentMap[ obj ];
  const QModelIndex parentIndex = indexForObject(parentObj);
  if (parentObj && !parentIndex.isValid()) {
    return;
  }
  QVector<QObject*> &children = m_parentChildMap[ parentObj ];
  const int index = children.indexOf(obj);
  if (index < 0 || index >= children.size()) {
    return;
  }

  beginRemoveRows(parentIndex, index, index);
  children.remove(index);
  m_childParentMap.remove(obj);
  if (m_parentChildMap.value(obj).isEmpty()) {
    m_parentChildMap.remove(obj);
  }
  endRemoveRows();
}

QVariant ObjectTreeModel::data(const QModelIndex &index, int role) const
{
  ReadOrWriteLocker lock(&m_lock);
  QObject *obj = reinterpret_cast<QObject*>(index.internalPointer());
  if (obj) {
    return dataForObject(obj, index, role);
  }
  return QVariant();
}

int ObjectTreeModel::rowCount(const QModelIndex &parent) const
{
  ReadOrWriteLocker lock(&m_lock);
  QObject *parentObj = reinterpret_cast<QObject*>(parent.internalPointer());
  return m_parentChildMap.value(parentObj).size();
}

QModelIndex ObjectTreeModel::parent(const QModelIndex &child) const
{
  ReadOrWriteLocker lock(&m_lock);
  QObject *childObj = reinterpret_cast<QObject*>(child.internalPointer());
  return indexForObject(m_childParentMap.value(childObj));
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
  ReadOrWriteLocker lock(&m_lock);
  QObject *parentObj = reinterpret_cast<QObject*>(parent.internalPointer());
  const QVector<QObject*> children = m_parentChildMap.value(parentObj);
  if (row < 0 || column < 0 || row >= children.size()  || column >= columnCount()) {
    return QModelIndex();
  }
  return createIndex(row, column, children.at(row));
}

QModelIndex ObjectTreeModel::indexForObject(QObject *object) const
{
  if (!object) {
    return QModelIndex();
  }
  QObject *parent = m_childParentMap.value(object);
  const QModelIndex parentIndex = indexForObject(parent);
  if (!parentIndex.isValid() && parent) {
    return QModelIndex();
  }
  int row = m_parentChildMap[ parent ].indexOf(object);
  if (row < 0) {
    return QModelIndex();
  }
  return index(row, 0, parentIndex);
}

#include "objecttreemodel.moc"

