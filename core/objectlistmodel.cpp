/*
  objectlistmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "probe.h"
#include "readorwritelocker.h"

#include <QThread>

#include <algorithm>
#include <iostream>

using namespace GammaRay;
using namespace std;

ObjectListModel::ObjectListModel(Probe *probe)
  : ObjectModelBase< QAbstractTableModel >(probe)
{
  connect(probe, SIGNAL(objectCreated(QObject*)),
          this, SLOT(objectAdded(QObject*)));
  connect(probe, SIGNAL(objectDestroyed(QObject*)),
          this, SLOT(objectRemoved(QObject*)));
}

QVariant ObjectListModel::data(const QModelIndex &index, int role) const
{
  ReadOrWriteLocker lock(Probe::instance()->objectLock());
  if (index.row() >= 0 && index.row() < m_objects.size()) {
    QObject *obj = m_objects.at(index.row());
    if (Probe::instance()->isValidObject(obj)) {
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
  Q_ASSERT(QThread::currentThread() == thread());
  Q_ASSERT(obj);

  ReadOrWriteLocker lock(Probe::instance()->objectLock());
  if (!Probe::instance()->isValidObject(obj))
    return;

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
  Q_ASSERT(thread() == QThread::currentThread());

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

