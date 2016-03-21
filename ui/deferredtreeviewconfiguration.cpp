/*
  deferredtreeviewconfiguration.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "deferredtreeviewconfiguration.h"

#include <QTreeView>
#include <QTimer>
#include <iostream>

using namespace GammaRay;
using namespace std;

DeferredTreeViewConfiguration::DeferredTreeViewConfiguration(QTreeView *view, bool expandNewContent, QObject *parent)
  : QObject(parent ? parent : view)
  , m_view(view)
  , m_expand(expandNewContent)
  , m_allExpanded(false)
  , m_timer(new QTimer(this))
{
  Q_ASSERT(view);
  Q_ASSERT(view->model());

  m_timer->setSingleShot(true);
  m_timer->setInterval(125);

  connect(view->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          SLOT(rowsInserted(QModelIndex)));
  connect(view->model(), SIGNAL(columnsInserted(QModelIndex,int,int)),
          SLOT(columnsInserted(QModelIndex)));
  connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));

  if (view->model()->rowCount() > 0) {
    rowsInserted(QModelIndex());
    columnsInserted(QModelIndex());
  }
}

DeferredTreeViewConfiguration::~DeferredTreeViewConfiguration()
{
  m_timer->stop();
}

void DeferredTreeViewConfiguration::hideColumn(int column)
{
  m_hiddenColumns << column;

  columnsInserted(QModelIndex());
}

void DeferredTreeViewConfiguration::rowsInserted(const QModelIndex &parent)
{
  if (m_expand) {
    m_insertedRows << QPersistentModelIndex(parent);
    m_timer->start();
  }
}

void DeferredTreeViewConfiguration::columnsInserted(const QModelIndex &parent)
{
  if (m_hiddenColumns.isEmpty()) {
    return;
  }

  const int columns = m_view->model()->columnCount(parent);
  foreach(int column, m_hiddenColumns) {
    if (column < columns) {
      m_view->hideColumn(column);
    }
  }
}

void DeferredTreeViewConfiguration::timeout()
{
  const QModelIndex selectedRow = m_view->selectionModel()->selectedRows().value(0);

  if (m_allExpanded) {
    for (auto it = m_insertedRows.constBegin(), end = m_insertedRows.constEnd(); it != end; ++it) {
      if (it->isValid()) {
        m_view->expand(*it);
      }
    }
  } else {
    m_allExpanded = true;
    m_view->expandAll();
  }

  m_insertedRows.clear();

  if (selectedRow.isValid()) {
    m_view->scrollTo(selectedRow);
  }
}

