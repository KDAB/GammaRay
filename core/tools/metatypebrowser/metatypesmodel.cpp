/*
  metatypesmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QDebug>
#include <QMetaType>

MetaTypesModel::MetaTypesModel(QObject *parent)
  : QAbstractTableModel(parent)
{
    scanMetaTypes(); // TODO do we need to re-run this when new types are registered at runtime?
}

QVariant MetaTypesModel::data(const QModelIndex &index, int role) const
{
  if (role != Qt::DisplayRole || !index.isValid()) {
    return QVariant();
  }

  int metaTypeId = m_metaTypes.at(index.row());
  switch (index.column()) {
    case 0:
    {
      QString name(QMetaType::typeName(metaTypeId));
      if (name.isEmpty())
        return tr("N/A");
      return name;
    }
    case 1:
      return metaTypeId;
    // TODO type flags
  }
  return QVariant();
}

int MetaTypesModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }

  return m_metaTypes.size();
}

int MetaTypesModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return 2;
}

QVariant MetaTypesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  Q_UNUSED(orientation);

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  if (section == 0) {
    return tr("Type Name");
  }

  return tr("Meta Type Id");
}

void MetaTypesModel::scanMetaTypes()
{
  beginResetModel();
  m_metaTypes.clear();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  for (int mtId = 0; QMetaType::isRegistered(mtId); ++mtId)
    m_metaTypes.push_back(mtId);
#else
  for (int mtId = 0; mtId <= QMetaType::User || QMetaType::isRegistered(mtId); ++mtId) {
    if (QMetaType::isRegistered(mtId))
      m_metaTypes.push_back(mtId);
  }
#endif
  endResetModel();
}


#include "metatypesmodel.moc"
