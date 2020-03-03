/*
  objectmodelbase.h

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
  This file is part of the GammaRay Plugin API and declares a template for an ObjectModelBase class.

  @brief
  Declares a template for an ObjectModelBase class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_OBJECTMODELBASE_H
#define GAMMARAY_OBJECTMODELBASE_H

#include "util.h"
#include "objectdataprovider.h"

#include <common/objectid.h>
#include <common/objectmodel.h>
#include <common/sourcelocation.h>

#include <QCoreApplication>
#include <QModelIndex>
#include <QObject>

namespace GammaRay {
/*! A container for a generic Object Model derived from some Base. */
template<typename Base>
class ObjectModelBase : public Base
{
public:
    /*!
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit ObjectModelBase<Base>(QObject *parent) : Base(parent)
    {
    }

    /*!
     * Returns the number of columns in the specified model (currently this is
     * always 2).
     * @param parent is the model QModelIndex.
     * @return the column count for specified model.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return 2;
    }

    /*!
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
            if (index.column() == 0)
                return Util::shortDisplayString(object);
            else if (index.column() == 1)
                return ObjectDataProvider::typeName(object);
        } else if (role == ObjectModel::ObjectRole) {
            return QVariant::fromValue(object);
        } else if (role == ObjectModel::ObjectIdRole) {
            return QVariant::fromValue(ObjectId(object));
        } else if (role == Qt::ToolTipRole) {
            return Util::tooltipForObject(object);
        } else if (role == ObjectModel::DecorationIdRole && index.column() == 0) {
            const int id = Util::iconIdForObject(object);
            return id >= 0 ? QVariant(id) : QVariant();
        } else if (role == ObjectModel::CreationLocationRole) {
            const auto loc = ObjectDataProvider::creationLocation(object);
            if (loc.isValid())
                return QVariant::fromValue(loc);
        } else if (role == ObjectModel::DeclarationLocationRole) {
            const auto loc = ObjectDataProvider::declarationLocation(object);
            if (loc.isValid())
                return QVariant::fromValue(loc);
        }

        return QVariant();
    }

    QMap<int, QVariant> itemData(const QModelIndex &index) const override
    {
        QMap<int, QVariant> map = Base::itemData(index);
        map.insert(ObjectModel::ObjectIdRole, this->data(index, ObjectModel::ObjectIdRole));
        map.insert(ObjectModel::DecorationIdRole, this->data(index, ObjectModel::DecorationIdRole));
        auto loc = this->data(index, ObjectModel::CreationLocationRole);
        if (loc.isValid())
            map.insert(ObjectModel::CreationLocationRole, loc);
        loc = this->data(index, ObjectModel::DeclarationLocationRole);
        if (loc.isValid())
            map.insert(ObjectModel::DeclarationLocationRole, loc);
        return map;
    }

    /*!
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
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return qApp->translate("GammaRay::ObjectModelBase", "Object");
            case 1:
                return qApp->translate("GammaRay::ObjectModelBase", "Type");
            }
        }
        return Base::headerData(section, orientation, role);
    }
};
}

#endif
