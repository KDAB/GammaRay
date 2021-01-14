/*
  qtivipropertyclientmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#include "qtivipropertyclientmodel.h"
#include "qtivipropertymodel.h"

#include <QApplication>
#include <QFont>

using namespace GammaRay;

QtIviPropertyClientModel::QtIviPropertyClientModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

QtIviPropertyClientModel::~QtIviPropertyClientModel() = default;

QVariant QtIviPropertyClientModel::data(const QModelIndex &index, int role) const
{
    // QIviProperty's appears in italic to distinguish from plain qt properties
    if (role == Qt::FontRole) {
        const bool isIviProperty = QSortFilterProxyModel::data(index, QtIviPropertyModel::IsIviProperty).toBool();
        QFont font = QApplication::font("QAbstractItemView");
        font.setItalic(isIviProperty);
        return QVariant::fromValue(font);
    }

    return QSortFilterProxyModel::data(index, role);
}
