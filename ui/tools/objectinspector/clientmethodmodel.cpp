/*
  clientmethodmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "clientmethodmodel.h"

#include <common/metatypedeclarations.h>
#include <common/tools/objectinspector/methodmodel.h>

#include <QMetaMethod>

using namespace GammaRay;

ClientMethodModel::ClientMethodModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

ClientMethodModel::~ClientMethodModel()
{
}

QVariant ClientMethodModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.column() == 1 && role == Qt::DisplayRole) {
        const auto methodType = index.data(ObjectMethodModelRole::MetaMethodType).value<QMetaMethod::MethodType>();
        switch (methodType) {
            case QMetaMethod::Method:
                return tr("Method");
            case QMetaMethod::Constructor:
                return tr("Constructor");
            case QMetaMethod::Slot:
                return tr("Slot");
            case QMetaMethod::Signal:
                return tr("Signal");
            default:
                return tr("Unknown");
        }
    }
    if (index.column() != 1 && role == ObjectMethodModelRole::MetaMethodType)
        return index.sibling(index.row(), 1).data(ObjectMethodModelRole::MetaMethodType);
    if (role == Qt::ToolTipRole) {
        const auto idx = index.sibling(index.row(), 0);
        auto tt = idx.data(Qt::DisplayRole).toString();
        const auto tag = idx.data(ObjectMethodModelRole::MethodTag).toString();
        tt += tr("\nTag: %1\n").arg(tag.isEmpty() ?tr("<none>") : tag);
        const auto rev = idx.data(ObjectMethodModelRole::MethodRevision);
        if (!rev.isNull())
            tt += tr("Revision: %1").arg(rev.toInt());
        return tt;
    }
    if (role == ObjectMethodModelRole::MethodSortRole) {
        if (index.column() == 0)
            return index.data(ObjectMethodModelRole::MethodSignature);
        return index.data(Qt::DisplayRole);
    }

    return QIdentityProxyModel::data(index, role);
}

QVariant ClientMethodModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role != Qt::DisplayRole)
            return QVariant();
        switch (section) {
            case 0:
                return tr("Signature");
            case 1:
                return tr("Type");
            case 2:
                return tr("Access");
            case 3:
                return tr("Class");
        }
    }
    return QIdentityProxyModel::headerData(section, orientation, role);
}
