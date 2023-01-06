/*
  timezoneclientmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "timezoneclientmodel.h"
#include "timezonemodelroles.h"

#include <QApplication>
#include <QFont>
#include <QStyle>

using namespace GammaRay;

TimezoneClientModel::TimezoneClientModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

TimezoneClientModel::~TimezoneClientModel() = default;

QVariant TimezoneClientModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole && index.column() != 0 && index.column() != TimezoneModelColumns::StandardDisplayNameColumn) {
        return QIdentityProxyModel::data(index.sibling(index.row(), 0), role);
    } else if (role == Qt::DisplayRole && index.column() == TimezoneModelColumns::DSTColumn) {
        const auto v = QIdentityProxyModel::data(index, Qt::DisplayRole);
        const auto b = v.type() == QVariant::Bool && v.toBool();
        if (b && qApp->style()->standardIcon(QStyle::SP_DialogYesButton).isNull())
            return tr("yes");
        return QVariant();
    } else if (role == Qt::DecorationRole && index.column() == TimezoneModelColumns::DSTColumn) {
        const auto v = QIdentityProxyModel::data(index, Qt::DisplayRole);
        const auto b = v.type() == QVariant::Bool && v.toBool();
        return b ? qApp->style()->standardIcon(QStyle::SP_DialogYesButton) : QVariant();
    } else if (role == Qt::FontRole) {
        const auto v = QIdentityProxyModel::data(index.sibling(index.row(), 0), TimezoneModelRoles::LocalZoneRole);
        const auto b = v.type() == QVariant::Bool && v.toBool();
        if (b) {
            QFont f;
            f.setBold(true);
            return f;
        }
    }

    return QIdentityProxyModel::data(index, role);
}

QVariant TimezoneClientModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case TimezoneModelColumns::IanaIdColumn:
            return tr("IANA Id");
        case TimezoneModelColumns::CountryColumn:
            return tr("Country");
        case TimezoneModelColumns::StandardDisplayNameColumn:
            return tr("Standard Display Name");
        case TimezoneModelColumns::DSTColumn:
            return tr("DST");
        case TimezoneModelColumns::WindowsIdColumn:
            return tr("Windows Id");
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}
