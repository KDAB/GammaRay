/*
  objecttreemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "readorwritelocker.h"
#include "probe.h"

#include <QEvent>
#include <QThread>

#include <algorithm>
#include <iostream>

#define IF_DEBUG(x)

extern void dumpObject(QObject *);

using namespace std;
using namespace GammaRay;

ObjectTreeModel::ObjectTreeModel(Probe *probe)
  : ObjectModelBase< QAbstractItemModel >(probe)
{
  connect(probe, SIGNAL(objectCreated(QObject*)),
          this, SLOT(objectAdded(QObject*)));
  connect(probe, SIGNAL(objectDestroyed(QObject*)),
          this, SLOT(objectRemoved(QObject*)));
  connect(probe, SIGNAL(objectReparented(QObject*)),
          this, SLOT(objectReparented(QObject*)));
}

static inline QObject *parentObject(QObject *obj)
{
  return  obj->parent();
}

void ObjectTreeModel::objectAdded(QObject *obj)
{
  // slot, hence should always land in main thread due to auto connection
  Q_ASSERT(thread() == QThread::currentThread());

  ReadOrWriteLocker objectLock(Probe::instance()->objectLock());
  if (!Probe::instance()->isValidObject(obj)) {
    IF_DEBUG(cout << "tree invalid obj added: " << hex << obj << endl;)
    return;
  }
  IF_DEBUG(cout << "tree obj added: " << hex << obj << " p: " << parentObject(obj) << endl;)
  Q_ASSERT(!obj->parent() || Probe::instance()->isValidObject(parentObject(obj)));

  if (indexForObject(obj).isValid()) {
    IF_DEBUG(cout << "tree double obj added: " << hex << obj << endl;)
    return;
  }

  // this is ugly, but apparently it can happen
  // that an object gets created without parent
  // then later the delayed signal comes in
  // so catch this gracefully by first adding the
  // parent if required
  if (parentObject(obj)) {
    const QModelIndex index = indexForObject(parentObject(obj));
    if (!index.isValid()) {
      IF_DEBUG(cout << "tree: handle parent first" << endl;)
      objectAdded(parentObject(obj));
    }
  }

  const QModelIndex index = indexForObject(parentObject(obj));

  // either we get a proper parent and hence valid index or there is no parent
  Q_ASSERT(index.isValid() || !parentObject(obj));

  QVector<QObject*> &children = m_parentChildMap[ parentObject(obj) ];
  QVector<QObject*>::iterator it = std::lower_bound(children.begin(), children.end(), obj);
  const int row = std::distance(children.begin(), it);

  beginInsertRows(index, row, row);

  children.insert(it, obj);
  m_childParentMap.insert(obj, parentObject(obj));

  endInsertRows();
}

void ObjectTreeModel::objectRemoved(QObject *obj)
{
  // slot, hence should always land in main thread due to auto connection
  Q_ASSERT(thread() == QThread::currentThread());

  IF_DEBUG(cout
           << "tree removed: "
           << hex << obj << " "
           << hex << obj->parent() << dec << " "
           << m_parentChildMap.value(obj->parent()).size() << " "
           << m_parentChildMap.contains(obj) << endl;)

  if (!m_childParentMap.contains(obj)) {
    Q_ASSERT(!m_parentChildMap.contains(obj));
    return;
  }

  QObject *parentObj = m_childParentMap[ obj ];
  const QModelIndex parentIndex = indexForObject(parentObj);
  if (parentObj && !parentIndex.isValid()) {
    return;
  }

  QVector<QObject*> &siblings = m_parentChildMap[ parentObj ];

  QVector<QObject*>::iterator it = std::lower_bound(siblings.begin(), siblings.end(), obj);
  if (it == siblings.end() || *it != obj) {
    return;
  }
  const int row = std::distance(siblings.begin(), it);

  beginRemoveRows(parentIndex, row, row);

  siblings.erase(it);
  m_childParentMap.remove(obj);
  m_parentChildMap.remove(obj);

  endRemoveRows();
}

void ObjectTreeModel::objectReparented(QObject *obj)
{
  // slot, hence should always land in main thread due to auto connection
  Q_ASSERT(thread() == QThread::currentThread());

  ReadOrWriteLocker objectLock(Probe::instance()->objectLock());
  if (Probe::instance()->isValidObject(obj)) {
    objectAdded(obj);
  }

  objectRemoved(obj);
}

QVariant ObjectTreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  QObject *obj = reinterpret_cast<QObject*>(index.internalPointer());

  ReadOrWriteLocker lock(Probe::instance()->objectLock());
  if (Probe::instance()->isValidObject(obj)) {
    return dataForObject(obj, index, role);
  } else if (role == Qt::DisplayRole) {
    if (index.column() == 0) {
      return Util::addressToString(obj);
    } else {
      return tr("<deleted>");
    }
  }

  return QVariant();
}

int ObjectTreeModel::rowCount(const QModelIndex &parent) const
{
  if (parent.column() == 1) {
    return 0;
  }
  QObject *parentObj = reinterpret_cast<QObject*>(parent.internalPointer());
  return m_parentChildMap.value(parentObj).size();
}

QModelIndex ObjectTreeModel::parent(const QModelIndex &child) const
{
  QObject *childObj = reinterpret_cast<QObject*>(child.internalPointer());
  return indexForObject(m_childParentMap.value(childObj));
}

QModelIndex ObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
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
  const QVector<QObject*> &siblings = m_parentChildMap[ parent ];
  QVector<QObject*>::const_iterator it = std::lower_bound(siblings.constBegin(), siblings.constEnd(), object);
  if (it == siblings.constEnd() || *it != object) {
    return QModelIndex();
  }

  const int row = std::distance(siblings.constBegin(), it);
  return index(row, 0, parentIndex);
}

