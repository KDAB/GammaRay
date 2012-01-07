/*
  controlmodel.cpp

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

#include "controlmodel.h"

#include <QStyle>

using namespace GammaRay;

ControlModel::ControlModel(QObject* parent): QAbstractTableModel(parent), m_style(0)
{
}

void ControlModel::setStyle(QStyle* style)
{
  beginResetModel();
  m_style = style;
  endResetModel();
}

QVariant ControlModel::data(const QModelIndex& index, int role) const
{
  return QVariant();
}

int ControlModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 1;
}

int ControlModel::rowCount(const QModelIndex& parent) const
{
  if (!m_style || !parent.isValid())
    return 0;
  return 0;
}

QVariant ControlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "controlmodel.moc"
