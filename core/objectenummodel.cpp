/*
  objectenummodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "objectenummodel.h"

#include <QMetaEnum>
#include <QMetaObject>

using namespace GammaRay;

typedef MetaObjectModel<QMetaEnum,
                        &QMetaObject::enumerator,
                        &QMetaObject::enumeratorCount,
                        &QMetaObject::enumeratorOffset>
    SuperClass;

GammaRay::ObjectEnumModel::ObjectEnumModel(QObject *parent)
    : SuperClass(parent)
{
}

int ObjectEnumModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return SuperClass::rowCount(parent);
    if (parent.parent().isValid())
        return 0;
    const QMetaEnum e = m_metaObject->enumerator(parent.row());
    return e.keyCount();
}

int GammaRay::ObjectEnumModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QVariant ObjectEnumModel::data(const QModelIndex &index, int role) const
{
    if (!index.parent().isValid())
        return SuperClass::data(index, role);

    if (role == Qt::DisplayRole) {
        const QMetaEnum e = m_metaObject->enumerator(index.parent().row());
        if (index.column() == 0)
            return e.key(index.row());
        if (index.column() == 1)
            return e.value(index.row());
    }

    return QVariant();
}

QVariant ObjectEnumModel::metaData(const QModelIndex &index, const QMetaEnum &enumerator,
                                   int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return QString::fromLatin1(enumerator.name());
        if (index.column() == 1)
            return tr("%n element(s)", "", enumerator.keyCount());
    }
    return QVariant();
}

QString ObjectEnumModel::columnHeader(int index) const
{
    switch (index) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Value");
    }
    return QString();
}

QModelIndex GammaRay::ObjectEnumModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return SuperClass::index(row, column, parent);
    return createIndex(row, column, parent.row());
}

QModelIndex GammaRay::ObjectEnumModel::parent(const QModelIndex &child) const
{
    // note: Qt4 doesn't have qintptr
    if (static_cast<qptrdiff>(child.internalId()) == -1)
        return SuperClass::parent(child);
    return SuperClass::index(child.internalId(), 0, QModelIndex());
}
