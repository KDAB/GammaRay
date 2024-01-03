/*
  objectenummodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    if (parent.parent().isValid() || parent.column() > 0)
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
    if (!child.isValid())
        return {};
    if (static_cast<qintptr>(child.internalId()) == -1)
        return SuperClass::parent(child);
    return SuperClass::index(child.internalId(), 0, QModelIndex());
}
