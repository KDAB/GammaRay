/*
  objectlistmodel.cpp

  This file is part of Gammaray, the Qt application inspection and
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

#include "objectlistmodel.h"

#include "readorwritelocker.h"

#include <QThread>

using namespace Gammaray;

ObjectListModel::ObjectListModel(QObject *parent)
  : ObjectModelBase< QAbstractTableModel >(parent),
    m_lock(QReadWriteLock::Recursive)
{
}

QVariant ObjectListModel::data(const QModelIndex &index, int role) const
{
  ReadOrWriteLocker lock(&m_lock);
  if (index.row() >= 0 && index.row() < m_objects.size()) {
    QObject *obj = m_objects.at(index.row());
    if (m_objectsHash.value(obj, false)) {
      return dataForObject(obj, index, role);
    }
  }
  return QVariant();
}

int ObjectListModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return ObjectModelBase<QAbstractTableModel>::columnCount(parent);
}

int ObjectListModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }

  ReadOrWriteLocker lock(&m_lock);
  return m_objects.size();
}

void ObjectListModel::objectAdded(const QPointer<QObject> &objPtr)
{
  if (!objPtr) {
    return;
  }

  // when called from background, delay into foreground, otherwise call directly
  QMetaObject::invokeMethod(this, "objectAddedMainThread", Qt::AutoConnection,
                            Q_ARG(QPointer<QObject>, objPtr));
}

void ObjectListModel::objectAddedMainThread(const QPointer< QObject >& objPtr)
{
  if (!objPtr) {
    return;
  }

  QWriteLocker lock(&m_lock);

  QObject *obj = objPtr.data();
  if (m_objects.contains(obj)) {
    m_objectsHash[obj] = !objPtr.isNull();
    return;
  }

  beginInsertRows(QModelIndex(), m_objects.size(), m_objects.size());
  m_objects << obj;
  m_objectsHash[obj] = !objPtr.isNull();
  endInsertRows();
}

void ObjectListModel::objectRemoved(QObject *obj)
{
  if (thread() != QThread::currentThread()) {
    // invalidate data
    QWriteLocker lock(&m_lock);
    QHash< QObject *, bool >::iterator it = m_objectsHash.find(obj);
    if (it != m_objectsHash.end()) {
      it.value() = false;
    }
  }

  // when called from background, delay into foreground, otherwise call directly
  QMetaObject::invokeMethod(this, "objectRemovedMainThread", Qt::AutoConnection,
                            Q_ARG(QObject *, obj));
}

void ObjectListModel::objectRemovedMainThread(QObject *obj)
{
  QWriteLocker lock(&m_lock);

  m_objectsHash.remove(obj);

  const int index = m_objects.indexOf(obj);
  if (index == -1) {
    return;
  }

  beginRemoveRows(QModelIndex(), index, index);
  m_objects.remove(index);
  endRemoveRows();
}

bool ObjectListModel::isValidObject(QObject *obj) const
{
  ReadOrWriteLocker lock(&m_lock);

  return m_objectsHash.value(obj, false);
}

#include "objectlistmodel.moc"
