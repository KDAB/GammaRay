/*
  objectvisualizermodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "objectvisualizermodel.h"

#include <core/util.h>

using namespace GammaRay;

ObjectVisualizerModel::ObjectVisualizerModel(QObject *parent)
    : KRecursiveFilterProxyModel(parent)
{
}

ObjectVisualizerModel::~ObjectVisualizerModel() = default;

QVariant ObjectVisualizerModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (role == ObjectDisplayName) {
        QObject *obj = data(proxyIndex, ObjectModel::ObjectRole).value<QObject *>();
        return Util::displayString(obj);
    } else if (role == ObjectId) {
        QObject *obj = data(proxyIndex, ObjectModel::ObjectRole).value<QObject *>();
        return static_cast<qulonglong>(reinterpret_cast<quintptr>(obj));
    } else if (role == ClassName) {
        QObject *obj = data(proxyIndex, ObjectModel::ObjectRole).value<QObject *>();
        Q_ASSERT(obj);
        return obj->metaObject()->className();
    }

    return KRecursiveFilterProxyModel::data(proxyIndex, role);
}
