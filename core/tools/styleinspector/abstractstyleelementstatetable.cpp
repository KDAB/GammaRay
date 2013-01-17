/*
  abstractstyleelementstatetable.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QPainter>
#include <QStyleOption>

using namespace GammaRay;

AbstractStyleElementStateTable::AbstractStyleElementStateTable(QObject *parent)
  : AbstractStyleElementModel(parent),
    m_cellWidth(64),
    m_cellHeight(64),
    m_zoomFactor(1)
{
}

int AbstractStyleElementStateTable::doColumnCount() const
{
  return StyleOption::stateCount();
}

QVariant AbstractStyleElementStateTable::doData(int row, int column, int role) const
{
  Q_UNUSED(column);
  Q_UNUSED(row);
  if (role == Qt::SizeHintRole) {
    return QSize(cellWidth() * zoomFactor() + 4, cellHeight() * zoomFactor() + 4);
  }
  return QVariant();
}

QVariant AbstractStyleElementStateTable::headerData(int section,
                                                    Qt::Orientation orientation,
                                                    int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return StyleOption::stateDisplayName(section);
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

void AbstractStyleElementStateTable::drawTransparencyBackground(QPainter *painter,
                                                                const QRect &rect) const
{
  QPixmap bgPattern(16, 16);
  bgPattern.fill(Qt::lightGray);
  QPainter bgPainter(&bgPattern);
  bgPainter.fillRect(8, 0, 8, 8, Qt::gray);
  bgPainter.fillRect(0, 8, 8, 8, Qt::gray);

  QBrush bgBrush;
  bgBrush.setTexture(bgPattern);
  painter->fillRect(rect, bgBrush);
}

int AbstractStyleElementStateTable::cellWidth() const
{
  return m_cellWidth;
}

int AbstractStyleElementStateTable::cellHeight() const
{
  return m_cellHeight;
}

int AbstractStyleElementStateTable::zoomFactor() const
{
  return m_zoomFactor;
}

void AbstractStyleElementStateTable::setCellWidth(int width)
{
  m_cellWidth = width;
  if (rowCount() > 0) {
    emit dataChanged(index(0, 0), index(doRowCount() - 1, doColumnCount() - 1));
  }
}

void AbstractStyleElementStateTable::setCellHeight(int height)
{
  m_cellHeight = height;
  if (rowCount() > 0) {
    emit dataChanged(index(0, 0), index(doRowCount() - 1, doColumnCount() - 1));
  }
}

void AbstractStyleElementStateTable::setZoomFactor(int zoom)
{
  Q_ASSERT(zoom > 0);
  m_zoomFactor = zoom;
  if (rowCount() > 0) {
    emit dataChanged(index(0, 0), index(doRowCount() - 1, doColumnCount() - 1));
  }
}

QSize AbstractStyleElementStateTable::effectiveCellSize() const
{
  return QSize(cellWidth() * zoomFactor(), cellHeight() * zoomFactor());
}

void AbstractStyleElementStateTable::fillStyleOption(QStyleOption *option, int column) const
{
  option->rect = QRect(0, 0, cellWidth(), cellHeight());
  option->palette = m_style->standardPalette();
  option->state = StyleOption::prettyState(column);
}

#include "abstractstyleelementstatetable.moc"
