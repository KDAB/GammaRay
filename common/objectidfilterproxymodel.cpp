/*
  objectidfilterproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#include "objectidfilterproxymodel.h"
#include "objectmodel.h"

using namespace GammaRay;

ObjectIdsFilterProxyModel::ObjectIdsFilterProxyModel(QObject *parent)
    : KRecursiveFilterProxyModel(parent)
{
}

void ObjectIdsFilterProxyModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    Q_UNUSED(order);
}

GammaRay::ObjectIds ObjectIdsFilterProxyModel::ids() const
{
    return m_ids;
}

void ObjectIdsFilterProxyModel::setIds(const GammaRay::ObjectIds &ids)
{
    if (m_ids == ids)
        return;

    m_ids = ids;
    invalidateFilter();
}

bool ObjectIdsFilterProxyModel::acceptRow(int source_row, const QModelIndex &source_parent) const
{
    // shortcut for the common case, the object id stuff below allocates memory and does expensive model lookups
    if (m_ids.isEmpty()) {
        return KRecursiveFilterProxyModel::acceptRow(source_row, source_parent);
    }

    const QModelIndex source_index = sourceModel()->index(source_row, 0, source_parent);
    if (!source_index.isValid()) {
        return false;
    }

    const GammaRay::ObjectId id = source_index.data(ObjectModel::ObjectIdRole).value<GammaRay::ObjectId>();
    if (id.isNull() || !filterAcceptsObjectId(id)) {
        return false;
    }

    return KRecursiveFilterProxyModel::acceptRow(source_row, source_parent);
}

bool ObjectIdsFilterProxyModel::filterAcceptsObjectId(const GammaRay::ObjectId &id) const
{
    return m_ids.contains(id);
}
