/*
  clientdecorationidentityproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
