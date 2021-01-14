/*
  objectenummodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectenummodel.h"

#include <QMetaEnum>
#include <QMetaObject>

using namespace GammaRay;

typedef MetaObjectModel<QMetaEnum,
                        &QMetaObject::enumerator,
                        &QMetaObject::enumeratorCount,
                        &QMetaObject::enumeratorOffset> SuperClass;

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
