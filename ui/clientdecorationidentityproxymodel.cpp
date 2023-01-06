/*
  clientdecorationidentityproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#include "clientdecorationidentityproxymodel.h"

#include <common/classesiconsrepository.h>
#include <common/objectmodel.h>
#include <common/objectbroker.h>

#include <common/endpoint.h>

#include <QDebug>

using namespace GammaRay;

ClientDecorationIdentityProxyModel::ClientDecorationIdentityProxyModel(QObject *parent)
    : QIdentityProxyModel(parent)
    , m_classesIconsRepository(ObjectBroker::object<ClassesIconsRepository *>())
{
}

QVariant ClientDecorationIdentityProxyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DecorationRole) {
        const QVariant decoration = QIdentityProxyModel::data(index, role);

        if (!decoration.isNull()) {
            return decoration;
        }

        if (m_classesIconsRepository) {
            const auto iconId = QIdentityProxyModel::data(index, ObjectModel::DecorationIdRole);
            if (iconId.isNull())
                return QVariant();
            const auto it = m_icons.constFind(iconId.toInt());
            if (it != m_icons.constEnd())
                return it.value();
            const auto icon = QIcon(m_classesIconsRepository->filePath(iconId.toInt()));
            if (icon.isNull())
                return QVariant();
            m_icons.insert(iconId.toInt(), icon);
            return QVariant::fromValue(icon);
        }

        return QVariant();
    }

    return QIdentityProxyModel::data(index, role);
}
