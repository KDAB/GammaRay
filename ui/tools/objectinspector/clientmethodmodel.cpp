/*
  clientmethodmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "clientmethodmodel.h"

#include <common/metatypedeclarations.h>
#include <common/qmetaobjectvalidatorresult.h>
#include <common/tools/objectinspector/methodmodel.h>

#include <QApplication>
#include <QMetaMethod>
#include <QStyle>

using namespace GammaRay;

ClientMethodModel::ClientMethodModel(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

ClientMethodModel::~ClientMethodModel() = default;

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
    if (index.column() == 2 && role == Qt::DisplayRole) {
        const auto methodAccess = index.data(ObjectMethodModelRole::MethodAccess).value<QMetaMethod::Access>();
        switch (methodAccess) {
        case QMetaMethod::Public:
            return tr("Public");
        case QMetaMethod::Protected:
            return tr("Protected");
        case QMetaMethod::Private:
            return tr("Private");
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
        tt += tr("\nTag: %1").arg(tag.isEmpty() ? tr("<none>") : tag);
        const auto rev = idx.data(ObjectMethodModelRole::MethodRevision);
        if (!rev.isNull())
            tt += tr("\nRevision: %1").arg(rev.toInt());

        const auto r = index.data(ObjectMethodModelRole::MethodIssues).value<QMetaObjectValidatorResult::Results>();
        if (r != QMetaObjectValidatorResult::NoIssue) {
            QStringList l;
            if (r & QMetaObjectValidatorResult::SignalOverride)
                l.push_back(tr("overrides base class signal"));
            if (r & QMetaObjectValidatorResult::UnknownMethodParameterType)
                l.push_back(tr("uses parameter type not registered with the meta type system"));
            tt += tr("\nIssues: %1").arg(l.join(", "));
        }
        return tt;
    }
    if (role == ObjectMethodModelRole::MethodSortRole) {
        if (index.column() == 0)
            return index.data(ObjectMethodModelRole::MethodSignature);
        return index.data(Qt::DisplayRole);
    }
    if (role == Qt::DecorationRole && index.column() == 0) {
        const auto r = index.data(ObjectMethodModelRole::MethodIssues).value<QMetaObjectValidatorResult::Results>();
        if (r != QMetaObjectValidatorResult::NoIssue)
            return qApp->style()->standardIcon(QStyle::SP_MessageBoxWarning);
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
