/*
  clientpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "clientpropertymodel.h"

#include <common/propertymodel.h>

#include <QStringList>

using namespace GammaRay;

ClientPropertyModel::ClientPropertyModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

ClientPropertyModel::~ClientPropertyModel() = default;

QVariant ClientPropertyModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::ToolTipRole && index.isValid()) {
        const auto idx0 = index.sibling(index.row(), 0);
        const auto f = idx0.data(PropertyModel::PropertyFlagsRole).value<PropertyModel::PropertyFlags>();
        const auto revision = idx0.data(PropertyModel::PropertyRevisionRole);
        const auto notifySignal = idx0.data(PropertyModel::NotifySignalRole).toString();

        QStringList tt;
        if (f != PropertyModel::None) {
            QStringList fs;
            if (f & PropertyModel::Constant)
                fs.push_back(tr("constant"));
            if (f & PropertyModel::Designable)
                fs.push_back(tr("designable"));
            if (f & PropertyModel::Final)
                fs.push_back(tr("final"));
            if (f & PropertyModel::Resetable)
                fs.push_back(tr("resetable"));
            if (f & PropertyModel::Scriptable)
                fs.push_back(tr("scriptable"));
            if (f & PropertyModel::Stored)
                fs.push_back(tr("stored"));
            if (f & PropertyModel::User)
                fs.push_back(tr("user"));
            if (f & PropertyModel::Writable)
                fs.push_back(tr("writable"));
            tt.push_back(tr("Attributes: %1").arg(fs.join(QLatin1String(", "))));
        }
        if (!revision.isNull()) {
            tt.push_back(tr("Revision: %1").arg(revision.toInt()));
        }
        if (!notifySignal.isEmpty()) {
            tt.push_back(tr("Notify signal: %1").arg(notifySignal));
        }

        return tt.join(QLatin1String("\n"));
    }
    return QIdentityProxyModel::data(index, role);
}

QVariant ClientPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Property");
        case 1:
            return tr("Value");
        case 2:
            return tr("Type");
        case 3:
            return tr("Class");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}
