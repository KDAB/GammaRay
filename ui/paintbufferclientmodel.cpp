/*
  paintbufferclientmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "paintbufferclientmodel.h"

#include <ui/uiintegration.h>

#include <common/paintbuffermodelroles.h>

#include <QColor>
#include <QDebug>

using namespace GammaRay;

PaintBufferClientModel::PaintBufferClientModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

PaintBufferClientModel::~PaintBufferClientModel() = default;

QVariant PaintBufferClientModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && !index.parent().isValid() && index.column() == 2) {
        switch (role) {
        case Qt::DisplayRole: {
            const auto cost = QIdentityProxyModel::data(index, Qt::DisplayRole).toDouble();
            if (cost < 0.005)
                return QVariant();
            return tr("%1 %").arg(qRound(cost * 100.0) / 100.0);
        }
        case Qt::BackgroundRole: {
            const auto cost = QIdentityProxyModel::data(index, Qt::DisplayRole).toDouble();
            if (cost < 0.005)
                return QVariant();
            const auto maxCost = QIdentityProxyModel::data(index.sibling(0, index.column()), PaintBufferModelRoles::MaxCostRole).toDouble();
            if (maxCost == 0.0)
                return QVariant();
            return QColor::fromHsv(120.0 * (1.0 - cost / maxCost), UiIntegration::hasDarkUI() ? 255 : 128, UiIntegration::hasDarkUI() ? 128 : 255);
        }
        }
    }
    return QIdentityProxyModel::data(index, role);
}

QVariant PaintBufferClientModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Command");
        case 1:
            return tr("Arguments");
        case 2:
            return tr("Cost");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
