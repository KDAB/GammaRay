/*
  singlecolumnobjectproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
