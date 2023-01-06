/*
  connectionsclientproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "connectionsclientproxymodel.h"

#include <common/tools/objectinspector/connectionsmodelroles.h>

#include <QApplication>
#include <QStyle>

using namespace GammaRay;

ConnectionsClientProxyModel::ConnectionsClientProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

ConnectionsClientProxyModel::~ConnectionsClientProxyModel() = default;

QVariant ConnectionsClientProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DecorationRole && index.column() == 0) {
        const bool warning = data(index, ConnectionsModelRoles::WarningFlagRole).toBool();
        if (warning)
            return qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning);
    }
    return QSortFilterProxyModel::data(index, role);
}
