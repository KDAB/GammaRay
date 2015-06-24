/*
  objectstaticpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include "objectstaticpropertymodel.h"
#include "varianthandler.h"
#include "util.h"
#include "probe.h"
#include "toolmodel.h"
#include "toolfactory.h"
#include "metaobjectrepository.h"

#include <common/propertymodel.h>

#include <QMetaProperty>
#include <QStringList>

using namespace GammaRay;

ObjectStaticPropertyModel::ObjectStaticPropertyModel(QObject *parent)
  : ObjectPropertyModel(parent)
{
}

static QString translateBool(bool value)
{
  return value ? QObject::tr("yes") : QObject::tr("no");
}

void ObjectStaticPropertyModel::setMetaObject(const QMetaObject* mo)
{
  setObject(0);
  if (mo == m_metaObject)
    return;

  beginResetModel();
  m_metaObject = mo;
  endResetModel();
}

QVariant ObjectStaticPropertyModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || !m_metaObject || index.row() < 0 ||
      index.row() >= m_metaObject->propertyCount()) {
    return QVariant();
  }

  const QMetaProperty prop = m_metaObject->property(index.row());
  const QVariant value = m_obj ? prop.read(m_obj.data()) : QVariant();
  if (role == Qt::DisplayRole) {
    if (index.column() == propertyColumnIndex()) {
      return prop.name();
    } else if (index.column() == valueColumnIndex()) {
      // QMetaProperty::read sets QVariant::typeName to int for enums,
      // so we need to handle that separately here
      const QString enumStr = Util::enumToString(value, prop.typeName(), m_obj.data());
      if (!enumStr.isEmpty()) {
        return enumStr;
      }
      return VariantHandler::displayString(value);
    } else if (index.column() == typeColumnIndex()) {
      return prop.typeName();
    } else if (index.column() == classColumnIndex()) {
      const QMetaObject *mo = m_metaObject;
      while (mo->propertyOffset() > index.row()) {
        mo = mo->superClass();
      }
      return mo->className();
    }
  } else if (role == Qt::DecorationRole) {
    if (index.column() == valueColumnIndex()) {
      return VariantHandler::decoration(value);
    }
  } else if (role == Qt::EditRole) {
    if (index.column() == valueColumnIndex()) {
      return value;
    }
  } else if (role == Qt::ToolTipRole) {
    return detailString(prop);
  } else if (role == PropertyModel::ActionRole) {
    if (!m_obj)
      return PropertyModel::NoAction;
    return (prop.isResettable() ? PropertyModel::Reset : PropertyModel::NoAction)
         | ((MetaObjectRepository::instance()->metaObject(value.typeName()) && *reinterpret_cast<void* const*>(value.data())) || value.value<QObject*>()
            ? PropertyModel::NavigateTo
            : PropertyModel::NoAction);
  } else if (role == PropertyModel::ValueRole) {
    return value;
  } else if (role == PropertyModel::AppropriateToolRole) {
    ToolModel *toolModel = Probe::instance()->toolModel();
    ToolFactory *factory;
    if (value.canConvert<QObject*>())
      factory = toolModel->data(toolModel->toolForObject(value.value<QObject*>()), ToolModelRole::ToolFactory).value<ToolFactory*>();
    else
      factory = toolModel->data(toolModel->toolForObject(*reinterpret_cast<void* const*>(value.data()), value.typeName()), ToolModelRole::ToolFactory).value<ToolFactory*>();
    if (factory) {
      return factory->name();
    }
    return QVariant();
  }

  return QVariant();
}

bool ObjectStaticPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.isValid() && m_obj && index.column() == valueColumnIndex() && index.row() >= 0 &&
      index.row() < m_obj.data()->metaObject()->propertyCount() && role == Qt::EditRole) {
    const QMetaProperty prop = m_obj.data()->metaObject()->property(index.row());
    const bool result = prop.write(m_obj.data(), value);
    if (result) {
      emit dataChanged(index, index);
    }
    return result;
  }
  return ObjectPropertyModel::setData(index, value, role);
}

int ObjectStaticPropertyModel::rowCount(const QModelIndex &parent) const
{
  if (!m_metaObject || parent.isValid()) {
    return 0;
  }
  return m_metaObject->propertyCount();
}

int ObjectStaticPropertyModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return isObjectMode() ? 4 : 3;
}

Qt::ItemFlags ObjectStaticPropertyModel::flags(const QModelIndex &index) const
{
  const Qt::ItemFlags flags = ObjectPropertyModel::flags(index);

  if (!index.isValid() || !m_obj || index.column() != valueColumnIndex() || index.row() < 0 ||
      index.row() >= m_obj.data()->metaObject()->propertyCount()) {
    return flags;
  }

  const QMetaProperty prop = m_obj.data()->metaObject()->property(index.row());
  if (prop.isWritable()) {
    return flags | Qt::ItemIsEditable;
  }
  return flags;
}

QVariant ObjectStaticPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    if (section == propertyColumnIndex())
      return tr("Property");
    if (section == valueColumnIndex())
      return tr("Value");
    if (section == typeColumnIndex())
      return tr("Type");
    if (section == classColumnIndex())
      return tr("Class");
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

void ObjectStaticPropertyModel::monitorObject(QObject* obj)
{
  for (int i = 0; i < obj->metaObject()->propertyCount(); ++i) {
    const QMetaProperty prop = obj->metaObject()->property(i);
    if (prop.hasNotifySignal()) {
      connect(obj, QByteArray("2") +
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
      prop.notifySignal().signature()
#else
      prop.notifySignal().methodSignature()
#endif
      , this, SLOT(propertyUpdated()));
      m_notifyToPropertyMap.insert(prop.notifySignalIndex(), i);
    }
  }
}

void ObjectStaticPropertyModel::unmonitorObject(QObject* obj)
{
  disconnect(obj, 0, this, SLOT(propertyUpdated()));
  m_notifyToPropertyMap.clear();
}

void ObjectStaticPropertyModel::propertyUpdated()
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
  Q_ASSERT(senderSignalIndex() >= 0);
  const int propertyIndex = m_notifyToPropertyMap.value(senderSignalIndex());
  emit dataChanged(index(propertyIndex, valueColumnIndex()), index(propertyIndex, valueColumnIndex()));
#else
  emit dataChanged(index(0,valueColumnIndex()), index(rowCount() - 1, valueColumnIndex()));
#endif
}

QString ObjectStaticPropertyModel::detailString(const QMetaProperty& prop) const
{
  QStringList s;
  s << tr("Constant: %1").arg(translateBool(prop.isConstant()));
  s << tr("Designable: %1").arg(translateBool(prop.isDesignable(m_obj.data())));
  s << tr("Final: %1").arg(translateBool(prop.isFinal()));
  if (prop.hasNotifySignal()) {
    s << tr("Notification: %1").arg(Util::prettyMethodSignature(prop.notifySignal()));
  } else {
    s << tr("Notification: no");
  }
  s << tr("Resetable: %1").arg(translateBool(prop.isResettable()));
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
  s << tr("Revision: %1").arg(prop.revision());
#endif
  s << tr("Scriptable: %1").arg(translateBool(prop.isScriptable(m_obj.data())));
  s << tr("Stored: %1").arg(translateBool(prop.isStored(m_obj.data())));
  s << tr("User: %1").arg(translateBool(prop.isUser(m_obj.data())));
  s << tr("Writable: %1").arg(translateBool(prop.isWritable()));
  return s.join("\n");
}

bool ObjectStaticPropertyModel::isObjectMode() const
{
  return m_obj || !m_metaObject;
}

int ObjectStaticPropertyModel::propertyColumnIndex() const
{
  return 0;
}

int ObjectStaticPropertyModel::valueColumnIndex() const
{
  return isObjectMode() ? 1: -1;
}

int ObjectStaticPropertyModel::typeColumnIndex() const
{
  return isObjectMode() ? 2 : 1;
}

int ObjectStaticPropertyModel::classColumnIndex() const
{
  return isObjectMode() ? 3 : 2;
}
