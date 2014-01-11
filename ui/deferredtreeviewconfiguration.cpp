/*
  deferredtreeviewconfiguration.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "deferredtreeviewconfiguration.h"

#include <QTreeView>
#include <iostream>

using namespace GammaRay;
using namespace std;

DeferredTreeViewConfiguration::DeferredTreeViewConfiguration(QTreeView *view, bool expandNewContent, bool selectNewContent, QObject *parent)
  : QObject(parent ? parent : view)
  , m_view(view)
  , m_expand(expandNewContent)
  , m_select(selectNewContent)
{
  Q_ASSERT(view);
  Q_ASSERT(view->model());
  Q_ASSERT(view->selectionModel());

  connect(view->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          SLOT(rowsInserted(QModelIndex)));
  connect(view->model(), SIGNAL(columnsInserted(QModelIndex,int,int)),
          SLOT(columnsInserted(QModelIndex)));

  if (view->model()->rowCount() > 0) {
    rowsInserted(QModelIndex());
    if (m_expand) {
      view->expandAll();
    }
  }
  columnsInserted(QModelIndex());
}

void DeferredTreeViewConfiguration::hideColumn(int column)
{
  m_hiddenColumns << column;

  columnsInserted(QModelIndex());
}

void DeferredTreeViewConfiguration::rowsInserted(const QModelIndex &parent)
{
  if (m_expand) {
    m_view->expand(parent);
  }
  if (m_select && !m_view->currentIndex().isValid()) {
    m_view->selectionModel()->setCurrentIndex(m_view->model()->index(0, 0), QItemSelectionModel::ClearAndSelect);
  }
}

void DeferredTreeViewConfiguration::columnsInserted(const QModelIndex &parent)
{
  if (m_hiddenColumns.isEmpty() || parent.isValid()) {
    return;
  }

  const int columns = m_view->model()->columnCount(parent);
  foreach(int column, m_hiddenColumns) {
    if (column < columns) {
      m_view->hideColumn(column);
    }
  }
}

