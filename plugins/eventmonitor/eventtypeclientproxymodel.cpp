/*
  eventtypeclientproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "eventtypeclientproxymodel.h"

#include "eventtypemodel.h"

#include <ui/uiintegration.h>

#include <QColor>

using namespace GammaRay;

EventTypeClientProxyModel::EventTypeClientProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

EventTypeClientProxyModel::~EventTypeClientProxyModel() = default;

// 1 / GRADIENT_SCALE_FACTOR is yellow, 2 / GRADIENT_SCALE_FACTOR and beyond is red
static const int GRADIENT_SCALE_FACTOR = 4;

static QColor colorForRatio(double ratio)
{
    const auto red = qBound<qreal>(0.0, ratio * GRADIENT_SCALE_FACTOR, 0.5);
    const auto green = qBound<qreal>(0.0, 1 - ratio * GRADIENT_SCALE_FACTOR, 0.5);
    auto color = QColor(int(255 * red), int(255 * green), 0);
    if (!UiIntegration::hasDarkUI())
        return color.lighter(300);
    return color;
}

QVariant EventTypeClientProxyModel::data(const QModelIndex &index, int role) const
{
    if (!sourceModel() || !index.isValid())
        return QVariant();

    if (role != Qt::BackgroundRole || index.column() != EventTypeModel::Count)
        return QIdentityProxyModel::data(index, role);

    const int maxCount = QIdentityProxyModel::data(index, EventTypeModel::MaxEventCount).toInt();
    const int count = QIdentityProxyModel::data(index, Qt::DisplayRole).toInt();
    if (maxCount <= 0 || count <= 0) {
        return QVariant();
    }
    const double ratio = double(count) / double(maxCount);

    return colorForRatio(ratio);
}

QVariant EventTypeClientProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case EventTypeModel::Columns::Type:
            return tr("Type");
        case EventTypeModel::Columns::Count:
            return tr("Count");
        case EventTypeModel::Columns::RecordingStatus:
            return tr("Record");
        case EventTypeModel::Columns::Visibility:
            return tr("Show");
        }
    }

    return QVariant();
}
