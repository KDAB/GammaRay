/*
  objectstaticpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectstaticpropertymodel.h"
#include "varianthandler.h"
#include "util.h"

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

QVariant ObjectStaticPropertyModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid() || !m_obj || index.row() < 0 ||
      index.row() >= m_obj.data()->metaObject()->propertyCount()) {
    return QVariant();
  }

  const QMetaProperty prop = m_obj.data()->metaObject()->property(index.row());
  if (role == Qt::DisplayRole) {
    if (index.column() == 0) {
      return prop.name();
    } else if (index.column() == 1) {
      // QMetaProperty::read sets QVariant::typeName to int for enums,
      // so we need to handle that separately here
      const QVariant value = prop.read(m_obj.data());
      const QString enumStr = Util::enumToString(value, prop.typeName(), m_obj.data());
      if (!enumStr.isEmpty()) {
        return enumStr;
      }
      return VariantHandler::displayString(value);
    } else if (index.column() == 2) {
      return prop.typeName();
    } else if (index.column() == 3) {
      const QMetaObject *mo = m_obj.data()->metaObject();
      while (mo->propertyOffset() > index.row()) {
        mo = mo->superClass();
      }
      return mo->className();
    }
  } else if (role == Qt::DecorationRole) {
    if (index.column() == 1) {
      return VariantHandler::decoration(prop.read(m_obj.data()));
    }
  } else if (role == Qt::EditRole) {
    if (index.column() == 1) {
      return prop.read(m_obj.data());
    }
  } else if (role == Qt::ToolTipRole) {
    return detailString(prop);
  } else if (role == PropertyModel::ActionRole) {
    return prop.isResettable() ? PropertyModel::Reset : PropertyModel::NoAction;
  }

  return QVariant();
}

bool ObjectStaticPropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.isValid() && m_obj && index.column() == 1 && index.row() >= 0 &&
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
  if (!m_obj || parent.isValid()) {
    return 0;
  }
  return m_obj.data()->metaObject()->propertyCount();
}

Qt::ItemFlags ObjectStaticPropertyModel::flags(const QModelIndex &index) const
{
  const Qt::ItemFlags flags = ObjectPropertyModel::flags(index);

  if (!index.isValid() || !m_obj || index.column() != 1 || index.row() < 0 ||
      index.row() >= m_obj.data()->metaObject()->propertyCount()) {
    return flags;
  }

  const QMetaProperty prop = m_obj.data()->metaObject()->property(index.row());
  if (prop.isWritable()) {
    return flags | Qt::ItemIsEditable;
  }
  return flags;
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
      , SLOT(updateAll()));
    }
  }
}

void ObjectStaticPropertyModel::unmonitorObject(QObject* obj)
{
  disconnect(obj, 0, this, SLOT(updateAll()));
}

QString ObjectStaticPropertyModel::detailString(const QMetaProperty& prop) const
{
  QStringList s;
  s << tr("Constant: %1").arg(translateBool(prop.isConstant()));
  s << tr("Designable: %1").arg(translateBool(prop.isDesignable(m_obj.data())));
  s << tr("Final: %1").arg(translateBool(prop.isFinal()));
  if (prop.hasNotifySignal()) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    s << tr("Notification: %1").arg(prop.notifySignal().signature());
#else
    s << tr("Notification: %1").arg(QString::fromUtf8(prop.notifySignal().methodSignature()));
#endif
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
