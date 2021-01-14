/*
  clientactionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "clientactionmodel.h"
#include "actionmodel.h" // for column/role enums

#include <QApplication>
#include <QStyle>

using namespace GammaRay;

ClientActionModel::ClientActionModel(QObject* parent)
    : QIdentityProxyModel(parent)
{
}

ClientActionModel::~ClientActionModel() = default;

QVariant ClientActionModel::data(const QModelIndex& index, int role) const
{
    if (role == ActionModel::ObjectIdRole && index.column() != ActionModel::AddressColumn) {
        return index.sibling(index.row(), ActionModel::AddressColumn).data(ActionModel::ObjectIdRole);
    } else if (role == Qt::DecorationRole && index.column() == ActionModel::ShortcutsPropColumn) {
        const auto v = index.data(ActionModel::ShortcutConflictRole);
        const auto b = v.type() == QVariant::Bool ? v.toBool() : false;
        return b ? qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning) : QVariant();
    } else if (role == Qt::ToolTipRole && index.column() == ActionModel::ShortcutsPropColumn) {
        const auto v = index.data(ActionModel::ShortcutConflictRole);
        const auto b = v.type() == QVariant::Bool ? v.toBool() : false;
        return b ? tr("Warning: Ambiguous shortcut detected.") : QVariant();
    }
    return QIdentityProxyModel::data(index, role);
}

QVariant ClientActionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role == Qt::DisplayRole) {
            switch (section) {
                case ActionModel::AddressColumn:
                    return tr("Object");
                case ActionModel::NameColumn:
                    return tr("Name");
                case ActionModel::CheckablePropColumn:
                    return tr("Checkable");
                case ActionModel::CheckedPropColumn:
                    return tr("Checked");
                case ActionModel::PriorityPropColumn:
                    return tr("Priority");
                case ActionModel::ShortcutsPropColumn:
                    return tr("Shortcut(s)");
                default:
                    return QVariant();
            }
        }
        return QVariant();
    }

    return QIdentityProxyModel::headerData(section, orientation, role);
}
