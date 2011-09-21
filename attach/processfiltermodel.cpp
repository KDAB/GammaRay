/*
  processfiltermodel.cpp

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "processfiltermodel.h"
#include "processmodel.h"

#include <QCoreApplication>

using namespace Endoscope;

ProcessFilterModel::ProcessFilterModel(QObject *parent)
  : QSortFilterProxyModel(parent)
{
  setFilterCaseSensitivity(Qt::CaseInsensitive);
  setFilterKeyColumn(1);

  m_currentProcId = QString::number(qApp->applicationPid());
}

bool ProcessFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
  const QString l = sourceModel()->data(left).toString();
  const QString r = sourceModel()->data(right).toString();
  if (left.column() == 0)
      return l.toInt() < r.toInt();
  return l < r;
}

bool ProcessFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
  ///TODO
  return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

Qt::ItemFlags ProcessFilterModel::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags = QSortFilterProxyModel::flags(index);

  if (index.data(ProcessModel::PIDRole).toString() == m_currentProcId) {
    return (flags & (~Qt::ItemIsEnabled));
  }

  return flags;
}
