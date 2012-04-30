/*
  resourcefiltermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
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

#include "resourcefiltermodel.h"

#include <qt/resourcemodel.h>

#include <QDebug>

using namespace GammaRay;

ResourceFilterModel::ResourceFilterModel(QObject *parent)
  : QSortFilterProxyModel(parent)
{
}

bool ResourceFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
  const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
  const QString path = index.data(ResourceModel::FilePathRole).toString();
  if (path == ":/gammaray" || path.startsWith(":/gammaray/")) {
    return false;
  }
  return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

#include "resourcefiltermodel.moc"
