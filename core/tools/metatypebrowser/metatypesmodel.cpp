/*
  metatypesmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QStringList>

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
    if (name.isEmpty()) {
      return tr("N/A");
    }
    return name;
  }
  case 1:
    return metaTypeId;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  case 2:
    return QMetaType::sizeOf(metaTypeId);
  case 3:
    return (QMetaType::metaObjectForType(metaTypeId) != 0);
  case 4:
  {
    const QMetaType::TypeFlags flags = QMetaType::typeFlags(metaTypeId);
    QStringList l;
    #define F(x) if (flags & QMetaType:: x) l.push_back(#x)
    F(NeedsConstruction);
    F(NeedsDestruction);
    F(MovableType);
    F(PointerToQObject);
    F(IsEnumeration);
    F(SharedPointerToQObject);
    F(WeakPointerToQObject);
    F(TrackingPointerToQObject);
    F(WasDeclaredAsMetaType);
    #undef F

    return l.join(", ");
  }
#endif
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
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  return 2;
#else
  return 5;
#endif
}

QVariant MetaTypesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole || orientation != Qt::Horizontal) {
    return QVariant();
  }

  switch (section) {
  case 0:
    return tr("Type Name");
  case 1:
    return tr("Meta Type Id");
  case 2:
    return tr("Size");
  case 3:
    return tr("QObject-derived");
  case 4:
    return tr("Type Flags");
  }
  return QVariant();
}

void MetaTypesModel::scanMetaTypes()
{
  beginResetModel();
  m_metaTypes.clear();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  for (int mtId = 0; QMetaType::isRegistered(mtId); ++mtId) {
    m_metaTypes.push_back(mtId);
  }
#else
  for (int mtId = 0; mtId <= QMetaType::User || QMetaType::isRegistered(mtId); ++mtId) {
    if (QMetaType::isRegistered(mtId)) {
      m_metaTypes.push_back(mtId);
    }
  }
#endif
  endResetModel();
}

