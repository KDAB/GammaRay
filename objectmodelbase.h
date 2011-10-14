/*
  objectmodelbase.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_OBJECTMODELBASE_H
#define GAMMARAY_OBJECTMODELBASE_H

#include "util.h"
#include "objectmodel.h"

#include <QObject>
#include <QModelIndex>

namespace GammaRay {

template<typename Base>
class ObjectModelBase : public Base
{
  public:
    explicit ObjectModelBase<Base>(QObject *parent) : Base(parent) {}

    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
      Q_UNUSED(parent);
      return 2;
    }

    QVariant dataForObject(QObject *obj, const QModelIndex &index, int role) const
    {
      if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
          return obj->objectName().isEmpty() ? Util::addressToString(obj) : obj->objectName();
        } else if (index.column() == 1) {
          return obj->metaObject()->className();
        }
      } else if (role == ObjectModel::ObjectRole) {
        return QVariant::fromValue(obj);
      } else if (role == Qt::ToolTipRole) {
          return QString("Object name: %1\nParent: %2 (Address: %3)\nNumber of children: %4").
            arg(obj->objectName().isEmpty() ? "<Not set>" : obj->objectName()).
            arg(obj->parent() ? obj->parent()->metaObject()->className() : "<No parent>").
            arg(Util::addressToString(obj->parent())).
            arg(obj->children().size());
      }

      return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
      if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
          return QObject::tr("Object");
        case 1:
          return QObject::tr("Type");
        }
      }
      return Base::headerData(section, orientation, role);
    }
};

}

#endif
