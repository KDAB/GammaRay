/*
  clientactionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "clientactionmodel.h"
#include "actionmodel.h" // for column/role enums

#include <QApplication>
#include <QStyle>

using namespace GammaRay;

ClientActionModel::ClientActionModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

ClientActionModel::~ClientActionModel() = default;

QVariant ClientActionModel::data(const QModelIndex &index, int role) const
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
