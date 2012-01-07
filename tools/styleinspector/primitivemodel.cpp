/*
  primitivemodel.cpp

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

#include "primitivemodel.h"
#include <qpixmap.h>
#include <qpainter.h>
#include <qstyleoption.h>

using namespace GammaRay;

PrimitiveModel::PrimitiveModel(QObject* parent): QAbstractTableModel(parent), m_style(0)
{
}

void PrimitiveModel::setStyle(QStyle* style)
{
  beginResetModel();
  m_style = style;
  endResetModel();
}

QVariant PrimitiveModel::data(const QModelIndex& index, int role) const
{
  if (!m_style || !index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole && index.column() == 0)
    return tr("PE_PanelButtonBevel");

  if (role == Qt::DecorationRole && index.column() == 1) {
    QPixmap pixmap(64,64);
    pixmap.fill(Qt::white); // TODO: use palette, or even better an alpha pattern
    QPainter painter(&pixmap);
    QStyleOption opt;
    opt.rect = pixmap.rect();
    m_style->drawPrimitive(QStyle::PE_PanelButtonBevel, &opt, &painter);
    return pixmap;
  }

  return QVariant();
}

int PrimitiveModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return 2;
}

int PrimitiveModel::rowCount(const QModelIndex& parent) const
{
  if (!m_style || parent.isValid())
    return 0;
  return 1;
}

QVariant PrimitiveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
      case 0: return tr("Primitive Element");
      case 1: return tr("Normal");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "primitivemodel.moc"
