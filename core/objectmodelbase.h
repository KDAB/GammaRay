/*
  objectmodelbase.h

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
/**
  @file
  This file is part of the GammaRay Plugin API and declares a template for an ObjectModelBase class.

  @brief
  Declares a template for an ObjectModelBase class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_OBJECTMODELBASE_H
#define GAMMARAY_OBJECTMODELBASE_H

#include "util.h"
#include <common/objectmodel.h>

#include <QModelIndex>
#include <QObject>

namespace GammaRay {

/**
 * @brief A container for a generic Object Model derived from some Base.
 */
template<typename Base>
class ObjectModelBase : public Base
{
  public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit ObjectModelBase<Base>(QObject *parent) : Base(parent)
    {
    }

    /**
     * Returns the number of columns in the specified model (currently this is
     * always 2).
     * @param parent is the model QModelIndex.
     * @return the column count for specified model.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
      Q_UNUSED(parent);
      return 2;
    }

    /**
     * Returns the data for the specified object.
     * @param object is a pointer to a QObject.
     * @param index is the model QModelIndex.
     * @param role is the Qt role.
     *
     * @return on success, a QVariant containing the data for the specified QObject;
     *         QVariant() if some anamoly occurs.
     */
    QVariant dataForObject(QObject *object, const QModelIndex &index, int role) const
    {
      if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
          return
            object->objectName().isEmpty() ?
              Util::addressToString(object) :
              object->objectName();
        } else if (index.column() == 1) {
          return object->metaObject()->className();
        }
      } else if (role == ObjectModel::ObjectRole) {
        return QVariant::fromValue(object);
      } else if (role == Qt::ToolTipRole) {
          return
            QObject::tr("<p style='white-space:pre'>Object name: %1\nType: %2\nParent: %3 (Address: %4)\nNumber of children: %5</p>").
              arg(object->objectName().isEmpty() ? "&lt;Not set&gt;" : object->objectName()).
              arg(object->metaObject()->className()).
              arg(object->parent() ? object->parent()->metaObject()->className() : "<No parent>").
              arg(Util::addressToString(object->parent())).
              arg(object->children().size());
      } else if (role == Qt::DecorationRole && index.column() == 0) {
        return Util::iconForObject(object);
      }

      return QVariant();
    }

    /**
     * Returns the header data for the Object, given a section (column),
     * orientation and role.
     * @param section an integer (either 0 or 1) corresponding to the section (column).
     * @param orientation is the Qt::Orientation.
     * @param role is the Qt role.
     *
     * @return on success, a QVariant containing the header data;
     *         QVariant() if some anamoly occurs.
     *
     */
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
