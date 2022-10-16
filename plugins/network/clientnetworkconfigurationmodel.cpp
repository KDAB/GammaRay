/*
  clientnetworkconfigurationmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "clientnetworkconfigurationmodel.h"
#include "networkconfigurationmodelroles.h"

#include <QFont>

using namespace GammaRay;

ClientNetworkConfigurationModel::ClientNetworkConfigurationModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

ClientNetworkConfigurationModel::~ClientNetworkConfigurationModel() = default;

QVariant ClientNetworkConfigurationModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::FontRole) {
        if (index.sibling(index.row(), 0).data(NetworkConfigurationModelRoles::DefaultConfigRole).toBool()) {
            QFont f;
            f.setBold(true);
            return f;
        }
    }
    return QIdentityProxyModel::data(index, role);
}

QVariant ClientNetworkConfigurationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Identifier");
        case 2:
            return tr("Bearer");
        case 3:
            return tr("Timeout");
        case 4:
            return tr("Roaming");
        case 5:
            return tr("Purpose");
        case 6:
            return tr("State");
        case 7:
            return tr("Type");
        }
    }
    return QIdentityProxyModel::headerData(section, orientation, role);
}
