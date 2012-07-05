/*
  objectmethodmodel.cpp

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

#include "objectmethodmodel.h"

using namespace GammaRay;

ObjectMethodModel::ObjectMethodModel(QObject *parent)
  : MetaObjectModel<QMetaMethod, &QMetaObject::method,
                    &QMetaObject::methodCount, &QMetaObject::methodOffset>(parent)
{
}

int GammaRay::ObjectMethodModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 4;
}

QVariant ObjectMethodModel::metaData(const QModelIndex &index,
                                 const QMetaMethod &method, int role) const
{
  if (role == Qt::DisplayRole) {
    if (index.column() == 0) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
      return method.signature();
#else
      return method.methodSignature();
#endif
    }
    if (index.column() == 1) {
      switch (method.methodType()) {
      case QMetaMethod::Method:
        return tr("Method");
      case QMetaMethod::Constructor:
        return tr("Constructor");
      case QMetaMethod::Slot:
        return tr("Slot");
      case QMetaMethod::Signal:
        return tr("Signal");
      default:
        return tr("Unknown");
      }
    }
    if (index.column() == 2) {
      switch (method.access()) {
      case QMetaMethod::Public:
        return tr("Public");
      case QMetaMethod::Protected:
        return tr("Protected");
      case QMetaMethod::Private:
        return tr("Private");
      default:
        return tr("Unknown");
      }
    }
  } else if (role == MetaMethodRole) {
    return QVariant::fromValue(method);
  }
  return QVariant();
}

QString GammaRay::ObjectMethodModel::columnHeader(int index) const
{
  switch (index) {
  case 0:
    return tr("Signature");
  case 1:
    return tr("Type");
  case 2:
    return tr("Access");
  }
  return QString();
}
