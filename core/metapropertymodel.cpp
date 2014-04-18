/*
  metapropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "varianthandler.h"
#include "toolmodel.h"
#include "probe.h"
#include "toolfactory.h"
#include <common/propertymodel.h>

using namespace GammaRay;

MetaPropertyModel::MetaPropertyModel(QObject *parent)
  : QAbstractTableModel(parent),
    m_metaObject(0),
    m_object(0)
{
}

void MetaPropertyModel::setObject(void *object, const QString &typeName)
{
  if (m_object == object)
    return;

  beginResetModel();
  m_object = object;
  m_metaObject = MetaObjectRepository::instance()->metaObject(typeName);
  endResetModel();
}

void MetaPropertyModel::setObject(QObject *object)
{
  if (m_object == object)
    return;

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

QMap< int, QVariant > MetaPropertyModel::itemData(const QModelIndex& index) const
{
  QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
  d.insert(PropertyModel::ActionRole, data(index, PropertyModel::ActionRole));
  d.insert(PropertyModel::AppropriateToolRole, data(index, PropertyModel::AppropriateToolRole));
  return d;
}

QVariant MetaPropertyModel::data(const QModelIndex &index, int role) const
{
  if (!m_metaObject || !index.isValid()) {
    return QVariant();
  }

  MetaProperty *property = m_metaObject->propertyAt(index.row());
  // TODO: cache this, to make this more robust against m_object becoming invalid
  const QVariant value = property->value(m_metaObject->castForPropertyAt(m_object, index.row()));

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

    switch (role) {
    case Qt::DisplayRole:
      return VariantHandler::displayString(value);
    case Qt::DecorationRole:
      return VariantHandler::decoration(value);
    case Qt::EditRole:
      return value;
    }
  }

  if (role == PropertyModel::ActionRole) {
    return (MetaObjectRepository::instance()->metaObject(property->typeName()) && *reinterpret_cast<void* const*>(value.data())) || value.value<QObject*>()
            ? PropertyModel::NavigateTo
            : PropertyModel::NoAction;
  } else if (role == PropertyModel::ValueRole) {
      return value;
  } else if (role == PropertyModel::AppropriateToolRole) {
    ToolModel *toolModel = Probe::instance()->toolModel();
    ToolFactory *factory;
    if (value.canConvert<QObject*>())
      factory = toolModel->data(toolModel->toolForObject(value.value<QObject*>()), ToolModelRole::ToolFactory).value<ToolFactory*>();
    else
      factory = toolModel->data(toolModel->toolForObject(*reinterpret_cast<void* const*>(value.data()), property->typeName()), ToolModelRole::ToolFactory).value<ToolFactory*>();

    if (factory)
      return factory->name();
    return QVariant();
  }
  return QVariant();
}

bool MetaPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.isValid() && index.column() == 1 && m_metaObject && m_object && role == Qt::EditRole) {
    MetaProperty *property = m_metaObject->propertyAt(index.row());
    property->setValue(m_metaObject->castForPropertyAt(m_object, index.row()), value);
    emit dataChanged(index, index);
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

