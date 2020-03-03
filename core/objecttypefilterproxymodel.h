/*
  objecttypefilterproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "gammaray_core_export.h"
#include "objectmodelbase.h"

#include <QSortFilterProxyModel>

namespace GammaRay {
/**
 * @brief A QSortFilterProxyModel for generic Objects.
 */
class GAMMARAY_CORE_EXPORT ObjectFilterProxyModelBase : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit ObjectFilterProxyModelBase(QObject *parent = nullptr);

    QMap<int, QVariant> itemData(const QModelIndex& index) const override;

protected:
    /**
     * Determines if the item in the specified row can be included in the model.
     * @param source_row is a non-zero integer representing the row of the item.
     * @param source_parent is the parent QModelIndex for this model.
     * @return true if the item in the row can be included in the model;
     *         otherwise returns false.
     */
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

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
template<typename T1, typename T2 = T1>
class ObjectTypeFilterProxyModel : public ObjectFilterProxyModelBase
{
public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit ObjectTypeFilterProxyModel(QObject *parent = nullptr)
        : ObjectFilterProxyModelBase(parent)
    {
    }

protected:
    bool filterAcceptsObject(QObject *object) const override
    {
        return qobject_cast<T1 *>(object) || qobject_cast<T2 *>(object);
    }
};

}

#endif // GAMMARAY_OBJECTTYPEFILTERPROXYMODEL_H
