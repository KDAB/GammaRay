/*
  objectlistmodel.cpp

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krauss@kdab.com>

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

using namespace Endoscope;

ObjectListModel::ObjectListModel(QObject* parent): ObjectModelBase< QAbstractTableModel >(parent)
{
}

QVariant ObjectListModel::data(const QModelIndex& index, int role) const
{
  if ( index.row() >= 0 && index.row() < m_objects.size() ) {
    QObject *obj = m_objects.at( index.row() );
    return dataForObject( obj, index, role );
  }
  return QVariant();
}

int ObjectListModel::columnCount(const QModelIndex& parent) const
{
  if ( parent.isValid() )
    return 0;
  return ObjectModelBase<QAbstractTableModel>::columnCount( parent );
}

int ObjectListModel::rowCount(const QModelIndex& parent) const
{
  if ( parent.isValid() )
    return 0;
  return m_objects.size();
}

void Endoscope::ObjectListModel::objectAdded( const QPointer<QObject> &objPtr )
{
  if ( !objPtr )
    return;
  QObject *obj = objPtr.data();
  const int index = m_objects.indexOf( obj );
  if ( index > 0 )
    return;
  beginInsertRows( QModelIndex(), m_objects.size(), m_objects.size() );
  m_objects.push_back( obj );
  endInsertRows();
}

void Endoscope::ObjectListModel::objectRemoved(QObject* obj)
{
  const int index = m_objects.indexOf( obj );
  if ( index < 0 || index >= m_objects.size() )
    return;
  beginRemoveRows( QModelIndex(), index, index );
  m_objects.remove( index );
  endRemoveRows();
}

#include "objectlistmodel.moc"
