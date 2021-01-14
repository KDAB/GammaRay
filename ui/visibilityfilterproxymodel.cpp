/*
  objectidfilterproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Manfred Tonch <manfred.tonch@kdab.com>

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

#include <visibilityfilterproxymodel.h>
#include "common/objectmodel.h"

#include <QDebug>

using namespace GammaRay;

VisibilityFilterProxyModel::VisibilityFilterProxyModel(QObject *parent)
    : KRecursiveFilterProxyModel(parent)
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

bool VisibilityFilterProxyModel::acceptRow(int source_row, const QModelIndex &source_parent) const
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

    return KRecursiveFilterProxyModel::acceptRow(source_row, source_parent);
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
