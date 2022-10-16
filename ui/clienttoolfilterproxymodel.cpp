/*
  clienttoolfilterproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "clienttoolfilterproxymodel.h"

#include <common/modelroles.h>

using namespace GammaRay;

ClientToolFilterProxyModel::ClientToolFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_filterInactiveTools(false)
{
}

ClientToolFilterProxyModel::~ClientToolFilterProxyModel() = default;

bool ClientToolFilterProxyModel::filterInactiveTools() const
{
    return m_filterInactiveTools;
}

void ClientToolFilterProxyModel::setFilterInactiveTools(bool enable)
{
    m_filterInactiveTools = enable;
    invalidate();
}

bool ClientToolFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_filterInactiveTools) {
        const auto toolEnabled = sourceModel()->data(sourceModel()->index(source_row, 0, source_parent), ToolModelRole::ToolEnabled).toBool();
        if (!toolEnabled)
            return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
