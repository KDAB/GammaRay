/*
  clientresourcemodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
                                                                              Qt::DisplayRole)
                                                                         .toString());
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
