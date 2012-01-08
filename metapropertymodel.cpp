/*
  metapropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "metapropertymodel.h"
#include "metaobjectrepository.h"
#include "metaobject.h"
#include "util.h"

using namespace GammaRay;

MetaPropertyModel::MetaPropertyModel(QObject *parent):
  QAbstractTableModel(parent),
  m_metaObject(0),
  m_object(0)
{
}

void MetaPropertyModel::setObject(void *object, const QString &typeName)
{
  beginResetModel();
  m_object = object;
  m_metaObject = MetaObjectRepository::instance()->metaObject(typeName);
  endResetModel();
}

void MetaPropertyModel::setObject(QObject *object)
{
  beginResetModel();
  m_object = 0;
  m_metaObject = 0;

  if (object) {
    const QMetaObject *mo = object->metaObject();
    while (mo && !m_metaObject) {
      m_metaObject = MetaObjectRepository::instance()->metaObject(mo->className());
      mo = mo->superClass();
    }
    if (m_metaObject) {
      m_object = object;
    }
  }
  endResetModel();
}

int MetaPropertyModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 4;
}

int MetaPropertyModel::rowCount(const QModelIndex &parent) const
{
  if (parent.isValid() || !m_metaObject) {
    return 0;
  }
  return m_metaObject->propertyCount();
}

QVariant MetaPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
    case 0:
      return tr("Property");
    case 1:
      return tr("Value");
    case 2:
      return tr("Type");
    case 3:
      return tr("Class");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

QVariant MetaPropertyModel::data(const QModelIndex &index, int role) const
{
  if (!m_metaObject || !index.isValid()) {
    return QVariant();
  }

  MetaProperty *property = m_metaObject->propertyAt(index.row());
  if (role == Qt::DisplayRole) {
    switch (index.column()) {
    case 0:
      return property->name();
    case 2:
      return property->typeName();
    case 3:
      return property->metaObject()->className();
    }
  }

  if (index.column() == 1) {
    if (!m_object) {
      return QVariant();
    }

    // TODO: cache this, to make this more robust against m_object becoming invalid
    const QVariant value = property->value(m_metaObject->castForPropertyAt(m_object, index.row()));
    switch (role) {
    case Qt::DisplayRole:
      return Util::variantToString(value);
    case Qt::DecorationRole:
      return Util::decorationForVariant(value);
    case Qt::EditRole:
      return value;
    }
  }
  return QVariant();
}

bool MetaPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.isValid() && index.column() == 1 && m_metaObject && m_object && role == Qt::EditRole) {
    MetaProperty *property = m_metaObject->propertyAt(index.row());
    property->setValue(m_metaObject->castForPropertyAt(m_object, index.row()), value);
    return true;
  }
  return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags MetaPropertyModel::flags(const QModelIndex &index) const
{
  const Qt::ItemFlags f = QAbstractItemModel::flags(index);
  if (!index.isValid() || index.column() != 1 || !m_metaObject || !m_object) {
    return f;
  }

  MetaProperty *property = m_metaObject->propertyAt(index.row());
  if (property->isReadOnly()) {
    return f;
  }
  return f | Qt::ItemIsEditable;
}

#include "metapropertymodel.moc"
