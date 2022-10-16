/*
  treeexpander.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "treeexpander.h"

#include <QScrollBar>
#include <QTreeView>

using namespace GammaRay;

TreeExpander::TreeExpander(QTreeView *view)
    : QObject(view)
    , m_view(view)
{
    Q_ASSERT(m_view);
    Q_ASSERT(m_view->model());

    connect(m_view->model(), &QAbstractItemModel::rowsInserted, this, &TreeExpander::rowsInserted);
}

TreeExpander::~TreeExpander()
{
}

void TreeExpander::rowsInserted(const QModelIndex &index, int start, int end)
{
    if (m_view->horizontalScrollBar()->isVisible()) {
        deleteLater();
        return;
    }

    if (index.isValid() && !m_view->isExpanded(index)) {
        return;
    }

    for (auto row = start; row <= end; ++row) {
        const auto idx = m_view->model()->index(row, 0, index);
        m_view->setExpanded(idx, true);
    }
}
