/*
  objectmethodmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "objectmethodmodel.h"
#include "util.h"

#include <core/qmetaobjectvalidator.h>
#include <common/tools/objectinspector/methodmodel.h>

using namespace GammaRay;

ObjectMethodModel::ObjectMethodModel(QObject *parent)
    : MetaObjectModel<QMetaMethod, &QMetaObject::method,
                      &QMetaObject::methodCount, &QMetaObject::methodOffset>(parent)
{
}

int GammaRay::ObjectMethodModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant ObjectMethodModel::metaData(const QModelIndex &index, const QMetaMethod &method,
                                     int role) const
{
    if (role == Qt::DisplayRole && index.column() == 0) {
        return Util::prettyMethodSignature(method);
    } else if (role == ObjectMethodModelRole::MetaMethod) {
        return QVariant::fromValue(method);
    } else if (role == ObjectMethodModelRole::MetaMethodType && index.column() == 1) {
        return QVariant::fromValue(method.methodType());
    } else if (role == ObjectMethodModelRole::MethodAccess && index.column() == 2) {
        return QVariant::fromValue(method.access());
    } else if (role == ObjectMethodModelRole::MethodSignature && index.column() == 0) {
        return method.methodSignature();
    } else if (role == ObjectMethodModelRole::MethodTag && index.column() == 0 && qstrlen(method.tag())) {
        return method.tag();
    } else if (role == ObjectMethodModelRole::MethodRevision && index.column() == 0) {
        return method.revision();
    } else if (role == ObjectMethodModelRole::MethodIssues && index.column() == 0) {
        const QMetaObject *mo = m_metaObject;
        while (mo->methodOffset() > index.row())
            mo = mo->superClass();
        const auto r = QMetaObjectValidator::checkMethod(mo, method);
        return r == QMetaObjectValidatorResult::NoIssue ? QVariant() : QVariant::fromValue(r);
    }
    return QVariant();
}

QMap<int, QVariant> ObjectMethodModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> m = super::itemData(index);
    m.insert(ObjectMethodModelRole::MetaMethodType, data(index, ObjectMethodModelRole::MetaMethodType));
    m.insert(ObjectMethodModelRole::MethodAccess, data(index, ObjectMethodModelRole::MethodAccess));
    m.insert(ObjectMethodModelRole::MethodSignature, data(index, ObjectMethodModelRole::MethodSignature));
    m.insert(ObjectMethodModelRole::MethodTag, data(index, ObjectMethodModelRole::MethodTag));
    m.insert(ObjectMethodModelRole::MethodRevision, data(index, ObjectMethodModelRole::MethodRevision));
    m.insert(ObjectMethodModelRole::MethodIssues, data(index, ObjectMethodModelRole::MethodIssues));
    return m;
}
