/*
  metatypesclientmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "metatypesclientmodel.h"

#include <QApplication>
#include <QStyle>

using namespace GammaRay;

MetaTypesClientModel::MetaTypesClientModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

MetaTypesClientModel::~MetaTypesClientModel() = default;

QVariant MetaTypesClientModel::data(const QModelIndex &index, int role) const
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
