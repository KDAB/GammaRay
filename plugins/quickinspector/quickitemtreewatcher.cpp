/*
  quickitemtreewatcher.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickitemtreewatcher.h"
#include "quickitemmodelroles.h"

#include <QAbstractItemModel>
#include <QTreeView>

using namespace GammaRay;

QuickItemTreeWatcher::QuickItemTreeWatcher(QTreeView *itemView, QTreeView *sgView, QObject *parent)
    : QObject(parent)
    , m_itemView(itemView)
    , m_sgView(sgView)
{
    connect(itemView->model(), &QAbstractItemModel::rowsInserted,
            this, &QuickItemTreeWatcher::itemModelRowsInserted);
    connect(sgView->model(), &QAbstractItemModel::rowsInserted,
            this, &QuickItemTreeWatcher::sgModelRowsInserted);
}

QuickItemTreeWatcher::~QuickItemTreeWatcher() = default;

void QuickItemTreeWatcher::itemModelRowsInserted(const QModelIndex &parent, int start, int end)
{
    if (parent.isValid() && !m_itemView->isExpanded(parent))
        return;

    const int siblingCount = m_itemView->model()->rowCount(parent);
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = m_itemView->model()->index(row, 0, parent);
        const bool invisible = index.data(QuickItemModelRole::ItemFlags).value<int>()
                               &(QuickItemModelRole::Invisible | QuickItemModelRole::ZeroSize);

        if (!invisible && siblingCount < 5)
            m_itemView->setExpanded(index, true);
    }

    m_itemView->resizeColumnToContents(0);
}

void QuickItemTreeWatcher::sgModelRowsInserted(const QModelIndex &parent, int start, int end)
{
    if (parent.isValid() && !m_sgView->isExpanded(parent))
        return;

    const int siblingCount = m_sgView->model()->rowCount(parent);
    for (int row = start; row <= end; ++row) {
        const QModelIndex index = m_sgView->model()->index(row, 0, parent);
        if (siblingCount < 5)
            m_sgView->setExpanded(index, true);
    }
    m_sgView->resizeColumnToContents(0);
}
