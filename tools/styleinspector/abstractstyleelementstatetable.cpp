/*
  abstractstyleelementstatetable.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "abstractstyleelementstatetable.h"
#include "styleoption.h"

using namespace GammaRay;

AbstractStyleElementStateTable::AbstractStyleElementStateTable(QObject* parent): AbstractStyleElementModel(parent)
{
}

int AbstractStyleElementStateTable::doColumnCount() const
{
  return 1 + StyleOption::stateCount();
}

QVariant AbstractStyleElementStateTable::doData(int row, int column, int role) const
{
  Q_UNUSED(row);
  if (role == Qt::SizeHintRole && column > 0)
    return QSize(68, 68);
  return QVariant();
}

QVariant AbstractStyleElementStateTable::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (section > 0 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return StyleOption::stateDisplayName(section - 1);
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "abstractstyleelementstatetable.moc"
