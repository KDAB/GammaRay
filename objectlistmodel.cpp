/*
  objectlistmodel.cpp

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

#include "objectlistmodel.h"

#include "readorwritelocker.h"

using namespace Endoscope;

ObjectListModel::ObjectListModel(QObject *parent)
  : ObjectModelBase< QAbstractTableModel >(parent)
  , m_lock(QReadWriteLock::Recursive)
{
}

QVariant ObjectListModel::data(const QModelIndex &index, int role) const
{
  ReadOrWriteLocker lock(&m_lock);
  if (index.row() >= 0 && index.row() < m_objects.size()) {
    QObject *obj = m_objects.at(index.row());
    return dataForObject(obj, index, role);
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
  QWriteLocker lock(&m_lock);

  if (!objPtr) {
    return;
  }

  QObject *obj = objPtr.data();
  const int index = m_objects.indexOf(obj);
  if (!objPtr || index > 0) {
    return;
  }

  beginInsertRows(QModelIndex(), m_objects.size(), m_objects.size());
  m_objects.push_back(obj);
  endInsertRows();
}

void ObjectListModel::objectRemoved(QObject *obj)
{
  QWriteLocker lock(&m_lock);

  const int index = m_objects.indexOf(obj);
  if (index < 0 || index >= m_objects.size()) {
    return;
  }
  beginRemoveRows(QModelIndex(), index, index);
  m_objects.remove(index);
  endRemoveRows();
}

#include "objectlistmodel.moc"
