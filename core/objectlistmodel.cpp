/*
  objectlistmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

using namespace GammaRay;

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
    if (obj) {
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

void ObjectListModel::objectAdded(QObject *obj)
{
  // when called from background, delay into foreground, otherwise call directly
  QMetaObject::invokeMethod(this, "objectAddedMainThread", Qt::AutoConnection,
                            Q_ARG(QObject *, obj));
}

void ObjectListModel::objectAddedMainThread(QObject *obj)
{
  ReadOrWriteLocker objectLock(Probe::instance()->objectLock());
  if (!Probe::instance()->isValidObject(obj)) {
    return;
  }

  QWriteLocker lock(&m_lock);
  if (m_objects.contains(obj)) {
    return;
  }

  beginInsertRows(QModelIndex(), m_objects.size(), m_objects.size());
  m_objects << obj;
  endInsertRows();
}

void ObjectListModel::objectRemoved(QObject *obj)
{
  if (thread() != QThread::currentThread()) {
    // invalidate data
    QWriteLocker lock(&m_lock);
    const int index = m_objects.indexOf(obj);
    if (index != -1) {
      m_objects[index] = 0;
    }
  }

  // when called from background, delay into foreground, otherwise call directly
  QMetaObject::invokeMethod(this, "objectRemovedMainThread", Qt::AutoConnection,
                            Q_ARG(QObject *, obj));
}

void ObjectListModel::objectRemovedMainThread(QObject *obj)
{
  QWriteLocker lock(&m_lock);

  for (int i = 0; i < m_objects.size(); ++i) {
    if (!m_objects.at(i) || m_objects.at(i) == obj) {
      beginRemoveRows(QModelIndex(), i, i);
      m_objects.remove(i);
      endRemoveRows();
    }
  }
}

#include "objectlistmodel.moc"
