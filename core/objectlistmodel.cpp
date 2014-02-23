/*
  objectlistmodel.cpp

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

#include "objectlistmodel.h"

#include "readorwritelocker.h"

#include <QThread>
#include "probe.h"

#include <iostream>

using namespace GammaRay;
using namespace std;

ObjectListModel::ObjectListModel(QObject *parent)
  : ObjectModelBase< QAbstractTableModel >(parent)
{
}

QVariant ObjectListModel::data(const QModelIndex &index, int role) const
{
  QMutexLocker lock(&m_mutex);
  if (index.row() >= 0 && index.row() < m_objects.size()) {
    QObject *obj = m_objects.at(index.row());
    if (!m_invalidatedObjects.contains(obj)) {
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

  return m_objects.size();
}

void ObjectListModel::objectAdded(QObject *obj)
{
  // when called from background, delay into foreground, otherwise call directly
  if (thread() != QThread::currentThread()) {
    {
      // revalidate data
      QMutexLocker lock(&m_mutex);
      m_invalidatedObjects.remove(obj);
    }
    QMetaObject::invokeMethod(this, "objectAddedMainThread", Qt::QueuedConnection,
                              Q_ARG(QObject*, obj));
  } else {
    objectAddedMainThread(obj);
  }
}

void ObjectListModel::objectAddedMainThread(QObject *obj)
{
  {
    QMutexLocker lock(&m_mutex);
    if (m_invalidatedObjects.contains(obj)) {
      return;
    }
  }

  Q_ASSERT(obj);

  QVector<QObject*>::iterator it = std::lower_bound(m_objects.begin(), m_objects.end(), obj);
  Q_ASSERT(it == m_objects.end() || *it != obj);

  const int row = std::distance(m_objects.begin(), it);
  Q_ASSERT(row >= 0 && row <= m_objects.size());

  beginInsertRows(QModelIndex(), row, row);
  m_objects.insert(it, obj);
  Q_ASSERT(m_objects.at(row) == obj);
  endInsertRows();
}

void ObjectListModel::objectRemoved(QObject *obj)
{
  // when called from background, delay into foreground, otherwise call directly
  if (thread() != QThread::currentThread()) {
    {
      // invalidate data
      QMutexLocker lock(&m_mutex);
      m_invalidatedObjects.insert(obj);
    }
    QMetaObject::invokeMethod(this, "objectRemovedMainThread", Qt::QueuedConnection,
                              Q_ARG(QObject*, obj), Q_ARG(bool, true));
  } else {
    objectRemovedMainThread(obj, false);
  }
}

void ObjectListModel::objectRemovedMainThread(QObject *obj, bool fromBackground)
{
  Q_ASSERT(thread() == QThread::currentThread());

  if (fromBackground) {
    QMutexLocker lock(&m_mutex);
    bool removed = m_invalidatedObjects.remove(obj);
    if (!removed) {
      Q_ASSERT(!m_objects.contains(obj));
      return;
    }
  } else {
#ifndef NDEBUG
    QMutexLocker lock(&m_mutex);
    Q_ASSERT(!m_invalidatedObjects.contains(obj));
#endif
  }

  if (m_objects.isEmpty()) {
    return;
  }

  QVector<QObject*>::iterator it = std::lower_bound(m_objects.begin(), m_objects.end(), obj);
  if (it == m_objects.end() || *it != obj) {
    // not found
    return;
  }

  const int row = std::distance(m_objects.begin(), it);
  Q_ASSERT(row >= 0 && row < m_objects.size());
  Q_ASSERT(m_objects.at(row) == obj);

  beginRemoveRows(QModelIndex(), row, row);
  m_objects.erase(it);
  endRemoveRows();
}

