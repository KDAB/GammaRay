/*
  metatypesclientmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "metatypesclientmodel.h"

#include <QApplication>
#include <QStyle>

using namespace GammaRay;

MetaTypesClientModel::MetaTypesClientModel(QObject* parent)
    : QIdentityProxyModel(parent)
{
}

MetaTypesClientModel::~MetaTypesClientModel() = default;

QVariant MetaTypesClientModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole && (index.column() == 5 || index.column() == 6)) {
        const auto v = QIdentityProxyModel::data(index, Qt::DisplayRole);
        const auto b = v.type() == QVariant::Bool && v.toBool();
        if (b && qApp->style()->standardIcon(QStyle::SP_DialogYesButton).isNull())
            return tr("yes");
        return QVariant();
    } else if (role == Qt::DecorationRole && (index.column() == 5 || index.column() == 6)) {
        const auto v = QIdentityProxyModel::data(index, Qt::DisplayRole);
        const auto b = v.type() == QVariant::Bool && v.toBool();
        return b ? qApp->style()->standardIcon(QStyle::SP_DialogYesButton) : QVariant();
    }
    return QIdentityProxyModel::data(index, role);
}

QVariant MetaTypesClientModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
                case 0:
                    return tr("Type Name");
                case 1:
                    return tr("Meta Type Id");
                case 2:
                    return tr("Size");
                case 3:
                    return tr("Meta Object");
                case 4:
                    return tr("Type Flags");
                case 5:
                    return tr("Compare");
                case 6:
                    return tr("Debug");
            }
        } else if (role == Qt::ToolTipRole) {
            switch (section) {
                case 5:
                    return tr("Has equality comparison operators registered.");
                case 6:
                    return tr("Has debug stream operators registered.");
            }
        }
        return QVariant();
    }
    return QIdentityProxyModel::headerData(section, orientation, role);
}
