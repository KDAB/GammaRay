/*
  abstractconnectionsmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "abstractconnectionsmodel.h"

#include <core/util.h>

#include <QMetaMethod>

using namespace GammaRay;

AbstractConnectionsModel::AbstractConnectionsModel(QObject* parent): QAbstractTableModel(parent)
{
}

AbstractConnectionsModel::~AbstractConnectionsModel()
{
}

int AbstractConnectionsModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 3;
}

int AbstractConnectionsModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return m_connections.size();
}

QVariant AbstractConnectionsModel::data(const QModelIndex& index, int role) const
{
  Q_UNUSED(index);
  Q_UNUSED(role);
  return QVariant();
}

QString AbstractConnectionsModel::displayString(QObject *object, int methodIndex)
{
  if (!object)
    return QObject::tr("<destroyed>");
  if (methodIndex < 0)
    return QObject::tr("<unknown>");

  const QMetaMethod method = object->metaObject()->method(methodIndex);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  return method.methodSignature();
#else
  return method.signature();
#endif
}

QString AbstractConnectionsModel::displayString(QObject* object)
{
  if (!object)
    return QObject::tr("<destroyed>");
  return Util::displayString(object);
}
