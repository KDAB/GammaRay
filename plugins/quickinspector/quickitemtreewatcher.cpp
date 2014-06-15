/*
  quickitemtreewatcher.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
#include <client/remotemodel.h>

#include <QAbstractItemModel>
#include <QTreeView>

using namespace GammaRay;

QuickItemTreeWatcher::QuickItemTreeWatcher(QTreeView *itemView, QTreeView *sgView, QObject *parent)
  : QObject(parent),
    m_itemView(itemView),
    m_sgView(sgView)
{
  connect(itemView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          this, SLOT(itemModelRowsInserted(QModelIndex,int,int)));
  connect(sgView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          this, SLOT(sgModelRowsInserted(QModelIndex,int,int)));
}

QuickItemTreeWatcher::~QuickItemTreeWatcher()
{
}

void QuickItemTreeWatcher::itemModelRowsInserted(const QModelIndex &parent, int start, int end)
{
  for (int row = start; row <= end; ++row) {
    const QModelIndex index = m_itemView->model()->index(row, 0, parent);
    const bool invisible = index.data(QuickItemModelRole::ItemFlags).value<int>() &
                           (QuickItemModelRole::Invisible | QuickItemModelRole::ZeroSize);
    const int siblingCount = m_itemView->model()->rowCount(parent);

    if (!invisible && siblingCount < 5) {
      m_itemView->setExpanded(index, true);
    }
  }
}

void QuickItemTreeWatcher::sgModelRowsInserted(const QModelIndex &parent, int start, int end)
{
  for (int row = start; row <= end; ++row) {
    const QModelIndex index = m_sgView->model()->index(row, 0, parent);
    const int siblingCount = m_sgView->model()->rowCount(parent);
    if (siblingCount < 5) {
      m_sgView->setExpanded(index, true);
    }
  }
  m_sgView->resizeColumnToContents(0);
}
