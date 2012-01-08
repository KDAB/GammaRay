/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "localeaccessormodel.h"
#include <qmath.h>
#include "localedataaccessor.h"

#include <QDebug>

using namespace GammaRay;

LocaleAccessorModel::LocaleAccessorModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

int LocaleAccessorModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  int area = LocaleDataAccessorRegistry::accessors().size();
  return qSqrt(area);
}

int LocaleAccessorModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  int area = LocaleDataAccessorRegistry::accessors().size();
  return qCeil((float)area / (int)qSqrt(area));
}

Qt::ItemFlags LocaleAccessorModel::flags(const QModelIndex &index) const
{
  return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant LocaleAccessorModel::data(const QModelIndex &index, int role) const
{
  QVector<LocaleDataAccessor*> acc = LocaleDataAccessorRegistry::accessors();
  int offset = (index.row() * columnCount()) + index.column();
  if (offset >= acc.size()) {
    return QVariant();
  }

  LocaleDataAccessor *accessor = acc.at(offset);
  switch(role) {
  case Qt::DisplayRole:
    return accessor->accessorName();
  case Qt::CheckStateRole:
    return
      LocaleDataAccessorRegistry::enabledAccessors().contains(accessor) ?
        Qt::Checked :
        Qt::Unchecked;
  case AccessorRole:
    return QVariant::fromValue<LocaleDataAccessor*>(accessor);
  default:
    return QVariant();
  }
}

bool LocaleAccessorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (role != Qt::CheckStateRole) {
    return QAbstractItemModel::setData(index, value, role);
  }
  bool enabled = value.toInt() == Qt::Checked;
  LocaleDataAccessor *accessor = index.data(AccessorRole).value<LocaleDataAccessor*>();
  LocaleDataAccessorRegistry::setAccessorEnabled(accessor, enabled);
  return false;
}

#include "localeaccessormodel.moc"
