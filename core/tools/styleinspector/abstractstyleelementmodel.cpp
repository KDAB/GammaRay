/*
  abstractstyleelementmodel.cpp

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

#include "abstractstyleelementmodel.h"

#include <QApplication>
#include <QProxyStyle>
#include <QStyle>

using namespace GammaRay;

AbstractStyleElementModel::AbstractStyleElementModel(QObject* parent): QAbstractTableModel(parent)
{
}

void AbstractStyleElementModel::setStyle(QStyle* style)
{
  beginResetModel();
  m_style = QPointer<QStyle>(style);
  endResetModel();
}

QVariant AbstractStyleElementModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid() || !m_style)
    return QVariant();
  return doData(index.row(), index.column(), role);
}

int AbstractStyleElementModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return doColumnCount();
}

int AbstractStyleElementModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid() || !m_style)
    return 0;
  return doRowCount();
}

bool AbstractStyleElementModel::isMainStyle() const
{
  QStyle *style = qApp->style();
  forever {
    if (style == m_style)
      return true;
    QProxyStyle *proxy = qobject_cast<QProxyStyle*>(style);
    if (!proxy)
      return false;
    style = proxy->baseStyle();
  }
}

#include "abstractstyleelementmodel.moc"
