/*
  objectstaticpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "include/util.h"

#include <QMetaProperty>

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
      return Util::variantToString(value);
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
      return Util::decorationForVariant(prop.read(m_obj.data()));
    }
  } else if (role == Qt::EditRole) {
    if (index.column() == 1) {
      return prop.read(m_obj.data());
    }
  } else if (role == Qt::ToolTipRole) {
    const QString toolTip =
      tr("Constant: %1\nDesignable: %2\nFinal: %3\nResetable: %4\n"
         "Has notification: %5\nScriptable: %6\nStored: %7\nUser: %8\nWritable: %9").
      arg(translateBool(prop.isConstant())).
      arg(translateBool(prop.isDesignable(m_obj.data()))).
      arg(translateBool(prop.isFinal())).
      arg(translateBool(prop.isResettable())).
      arg(translateBool(prop.hasNotifySignal())).
      arg(translateBool(prop.isScriptable(m_obj.data()))).
      arg(translateBool(prop.isStored(m_obj.data()))).
      arg(translateBool(prop.isUser(m_obj.data()))).
      arg(translateBool(prop.isWritable()));
    return toolTip;
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

int ObjectStaticPropertyModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return 0;
  }
  return 4;
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

#include "objectstaticpropertymodel.moc"
