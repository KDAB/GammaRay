/*
  quickitemtreewatcher.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
            & (QuickItemModelRole::Invisible | QuickItemModelRole::ZeroSize);

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
