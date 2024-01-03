/*
  visibilityfilterproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Manfred Tonch <manfred.tonch@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <visibilityfilterproxymodel.h>

#include <QDebug>

using namespace GammaRay;

VisibilityFilterProxyModel::VisibilityFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_hideItems(true)
    , m_flagRole(0)
    , m_invisibleMask(0)
{
}

void VisibilityFilterProxyModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    Q_UNUSED(order);
}

bool VisibilityFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex source_index = sourceModel()->index(source_row, 0, source_parent);
    if (!source_index.isValid()) {
        return false;
    }

    if (m_hideItems) {
        int flags = source_index.data(m_flagRole).toInt();
        if (flags & m_invisibleMask)
            return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

void VisibilityFilterProxyModel::setHideItems(bool hideItems)
{
    if (m_hideItems != hideItems) {
        m_hideItems = hideItems;
        invalidateFilter();
    }
}

void VisibilityFilterProxyModel::setFlagRole(int flagRole)
{
    if (m_flagRole != flagRole) {
        m_flagRole = flagRole;
        invalidateFilter();
    }
}


void VisibilityFilterProxyModel::setInvisibleMask(int invisibleMask)
{
    if (m_invisibleMask != invisibleMask) {
        m_invisibleMask = invisibleMask;
        invalidateFilter();
    }
}
