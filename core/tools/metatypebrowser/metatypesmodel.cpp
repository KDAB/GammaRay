/*
  metatypesmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "metatypesmodel.h"

#include <QtCore/QMetaType>
#include <QtCore/QDebug>

MetaTypesModel::MetaTypesModel(QObject *parent)
  : QAbstractItemModel(parent), m_lastMetaType(0)
{
  for (m_lastMetaType = 0; ; ++m_lastMetaType) {
    if (!QMetaType::isRegistered(m_lastMetaType)) {
      break;
    }
  }
}

QVariant MetaTypesModel::data(const QModelIndex &index, int role) const
{
  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  if (index.column() == 0) {
    return QMetaType::typeName(index.row());
  } else if (index.column() == 1) {
    return index.row();
  }
  return QVariant();
}

int MetaTypesModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }

  return m_lastMetaType;
}

int MetaTypesModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return 2;
}

QModelIndex MetaTypesModel::index(int row, int column, const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return createIndex(row, column);
}

QModelIndex MetaTypesModel::parent(const QModelIndex &child) const
{
  Q_UNUSED(child);
  return QModelIndex();
}

QVariant MetaTypesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(orientation);

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  if (section == 0) {
    return "typeName";
  }

  return "MetaTypeId";
}

#include "metatypesmodel.moc"
