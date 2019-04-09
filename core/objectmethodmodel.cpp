/*
  objectmethodmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

QMap< int, QVariant > ObjectMethodModel::itemData(const QModelIndex &index) const
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
