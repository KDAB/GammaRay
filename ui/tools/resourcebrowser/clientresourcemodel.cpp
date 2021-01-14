/*
  clientresourcemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include "clientresourcemodel.h"

#include <QIcon>

using namespace GammaRay;

ClientResourceModel::ClientResourceModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

ClientResourceModel::~ClientResourceModel() = default;

QVariant ClientResourceModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DecorationRole && index.column() == 0) {
        if (!index.parent().isValid())
            return m_iconProvider.icon(QFileIconProvider::Drive);
        if (hasChildren(index))
            return m_iconProvider.icon(QFileIconProvider::Folder);

        const QList<QMimeType> types = m_mimeDb.mimeTypesForFileName(index.data(
                                                                   Qt::DisplayRole).toString());
        for (const QMimeType &mt : types) {
            QIcon icon = QIcon::fromTheme(mt.iconName());
            if (!icon.isNull())
                return icon;
            icon = QIcon::fromTheme(mt.genericIconName());
            if (!icon.isNull())
                return icon;
        }
        return m_iconProvider.icon(QFileIconProvider::File);
    }
    return QIdentityProxyModel::data(index, role);
}
