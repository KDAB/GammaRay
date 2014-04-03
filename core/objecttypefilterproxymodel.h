/*
  objecttypefilterproxymodel.h

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
  This file is part of the GammaRay Plugin API and declares a template
  for an ObjectTypeFilterProxyModel class.

  @brief
  Declares a template for an ObjectTypeFilterProxyModel class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_OBJECTTYPEFILTERPROXYMODEL_H
#define GAMMARAY_OBJECTTYPEFILTERPROXYMODEL_H

#include "objectmodelbase.h"

#include <QSortFilterProxyModel>

namespace GammaRay {

/**
 * @brief A QSortFilterProxyModel for generic Objects.
 */
class ObjectFilterProxyModelBase : public QSortFilterProxyModel
{
  public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit ObjectFilterProxyModelBase(QObject *parent = 0) : QSortFilterProxyModel(parent)
    {
      setDynamicSortFilter(true);
    }

  protected:
    /**
     * Determines if the item in the specified row can be included in the model.
     * @param source_row is a non-zero integer representing the row of the item.
     * @param source_parent is the parent QModelIndex for this model.
     * @return true if the item in the row can be included in the model;
     *         otherwise returns false.
     */
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
    {
      const QModelIndex source_index = sourceModel()->index(source_row, 0, source_parent);
      if (!source_index.isValid()) {
        return false;
      }

      QObject *obj = source_index.data(ObjectModel::ObjectRole).value<QObject*>();
      if (!obj || !filterAcceptsObject(obj)) {
        return false;
      }

      return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }

    /**
     * Determines if the specified QObject can be included in the model.
     * @param object is a pointer to the QObject to test.
     * @return true if the QObject can be included in the model; false otherwise.
     */
    virtual bool filterAcceptsObject(QObject *object) const = 0;
};

/**
 * @brief A templated generic ObjectFilterProxyModelBase for some data type.
 */
template <typename T>
class ObjectTypeFilterProxyModel : public ObjectFilterProxyModelBase
{
  public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit ObjectTypeFilterProxyModel(QObject *parent = 0)
      : ObjectFilterProxyModelBase(parent)
    {
    }

  protected:
    virtual bool filterAcceptsObject(QObject *object) const
    {
      return qobject_cast<T*>(object);
    }
};

}

#endif // GAMMARAY_OBJECTTYPEFILTERPROXYMODEL_H
