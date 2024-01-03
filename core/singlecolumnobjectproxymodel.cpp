/*
  singlecolumnobjectproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "singlecolumnobjectproxymodel.h"

#include <core/util.h>
#include <common/objectmodel.h>

using namespace GammaRay;

SingleColumnObjectProxyModel::SingleColumnObjectProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

int SingleColumnObjectProxyModel::columnCount(const QModelIndex &parent) const
{
    return std::min(QIdentityProxyModel::columnCount(parent), 1);
}

QVariant SingleColumnObjectProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (proxyIndex.isValid() && role == Qt::DisplayRole && proxyIndex.column() == 0) {
        const QObject *obj = proxyIndex.data(ObjectModel::ObjectRole).value<QObject *>();
        if (obj)
            return Util::displayString(obj);
    }

    return QIdentityProxyModel::data(proxyIndex, role);
}

QMap<int, QVariant> SingleColumnObjectProxyModel::itemData(const QModelIndex &proxyIndex) const
{
    QMap<int, QVariant> map = QIdentityProxyModel::itemData(proxyIndex);
    map[Qt::DisplayRole] = data(proxyIndex);
    return map;
}
