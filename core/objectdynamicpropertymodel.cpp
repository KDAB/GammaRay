/*
  objectdynamicpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectdynamicpropertymodel.h"

using namespace GammaRay;

ObjectDynamicPropertyModel::ObjectDynamicPropertyModel(QObject *parent)
  : ObjectPropertyModel(parent)
{
}

QVariant ObjectDynamicPropertyModel::data(const QModelIndex &index, int role) const
{
  if (!m_obj) {
    return QVariant();
  }

  const QList<QByteArray> propNames = m_obj.data()->dynamicPropertyNames();
  if (index.row() < 0 || index.row() >= propNames.size()) {
    return QVariant();
  }

  const QByteArray propName = propNames.at(index.row());

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    const QVariant propValue = m_obj.data()->property(propName);
    if (index.column() == 0) {
      return QString::fromUtf8(propName);
    } else if (index.column() == 1) {
      return propValue;
    } else if (index.column() == 2) {
      return propValue.typeName();
    }
  }

  return QVariant();
}

bool ObjectDynamicPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (!m_obj) {
    return false;
  }

  const QList<QByteArray> propNames = m_obj.data()->dynamicPropertyNames();
  if (index.row() < 0 || index.row() >= propNames.size()) {
    return false;
  }

  if (role == Qt::EditRole) {
    const QByteArray propName = propNames.at(index.row());
    m_obj.data()->setProperty(propName, value);
    return true;
  }

  return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags ObjectDynamicPropertyModel::flags(const QModelIndex &index) const
{
  const Qt::ItemFlags flags = ObjectPropertyModel::flags(index);

  if (!index.isValid() || !m_obj || index.column() != 1) {
    return flags;
  }

  return flags | Qt::ItemIsEditable;
}

int ObjectDynamicPropertyModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return 3;
}

int ObjectDynamicPropertyModel::rowCount(const QModelIndex &parent) const
{
  if (!m_obj || parent.isValid()) {
    return 0;
  }
  return m_obj.data()->dynamicPropertyNames().size();
}

#include "objectdynamicpropertymodel.moc"
