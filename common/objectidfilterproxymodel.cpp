/*
  objectidfilterproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
