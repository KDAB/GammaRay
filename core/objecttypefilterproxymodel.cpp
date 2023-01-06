/*
  objecttypefilterproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "objecttypefilterproxymodel.h"

using namespace GammaRay;

ObjectFilterProxyModelBase::ObjectFilterProxyModelBase(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

QMap<int, QVariant> ObjectFilterProxyModelBase::itemData(const QModelIndex &index) const
{
    return sourceModel()->itemData(mapToSource(index));
}

bool ObjectFilterProxyModelBase::filterAcceptsRow(int source_row,
                                                  const QModelIndex &source_parent) const
{
    const QModelIndex source_index = sourceModel()->index(source_row, 0, source_parent);
    if (!source_index.isValid())
        return false;

    QObject *obj = source_index.data(ObjectModel::ObjectRole).value<QObject *>();
    if (!obj || !filterAcceptsObject(obj))
        return false;

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}
