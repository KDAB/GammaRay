/*
  objectidfilterproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "objectidfilterproxymodel.h"
#include "objectmodel.h"

using namespace GammaRay;

ObjectIdsFilterProxyModel::ObjectIdsFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
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

bool ObjectIdsFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // shortcut for the common case, the object id stuff below allocates memory and does expensive model lookups
    if (m_ids.isEmpty()) {
        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }

    const QModelIndex source_index = sourceModel()->index(source_row, 0, source_parent);
    if (!source_index.isValid()) {
        return false;
    }

    const GammaRay::ObjectId id = source_index.data(ObjectModel::ObjectIdRole).value<GammaRay::ObjectId>();
    if (id.isNull() || !filterAcceptsObjectId(id)) {
        return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool ObjectIdsFilterProxyModel::filterAcceptsObjectId(const GammaRay::ObjectId &id) const
{
    return m_ids.contains(id);
}
