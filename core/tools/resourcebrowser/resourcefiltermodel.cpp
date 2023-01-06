/*
  resourcefiltermodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "resourcefiltermodel.h"

#include "qt/resourcemodel.h"

#include <QDebug>

using namespace GammaRay;

ResourceFilterModel::ResourceFilterModel(QObject *parent)
    : KRecursiveFilterProxyModel(parent)
{
}

bool ResourceFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    const QString path = index.data(ResourceModel::FilePathRole).toString();
    if (path == QLatin1String(":/gammaray") || path.startsWith(QLatin1String(":/gammaray/")))
        return false;
    return KRecursiveFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
