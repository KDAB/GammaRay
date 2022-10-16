/*
  metatypesmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "metatypesmodel.h"

#include <core/metaobjectregistry.h>
#include <core/util.h>

#include <common/objectid.h>
#include <common/tools/metatypebrowser/metatyperoles.h>

#include <QDebug>
#include <QMetaType>
#include <QStringList>

#include <cstring>

using namespace GammaRay;

MetaTypesModel::MetaTypesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    scanMetaTypes(); // TODO do we need to re-run this when new types are registered at runtime?
}

QVariant MetaTypesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto metaTypeId = m_metaTypes.at(index.row());
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: {
            QString name(QMetaType::typeName(metaTypeId));
            if (name.isEmpty())
                return tr("N/A");
            return name;
        }
        case 1:
            return metaTypeId;
        case 2:
            return QMetaType::sizeOf(metaTypeId);
        case 3:
            return Util::addressToString(QMetaType::metaObjectForType(metaTypeId));
        case 4: {
            const QMetaType::TypeFlags flags = QMetaType::typeFlags(metaTypeId);
            QStringList l;
#define F(x)                  \
    if (flags & QMetaType::x) \
    l.push_back(QStringLiteral(#x))
            F(NeedsConstruction);
            F(NeedsDestruction);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            F(RelocatableType);
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
            F(IsConst);
#endif
            F(IsQmlList);
            F(IsUnsignedEnumeration);
#else
            F(MovableType);
            F(WasDeclaredAsMetaType);
#endif
            F(PointerToQObject);
            F(IsEnumeration);
            F(SharedPointerToQObject);
            F(WeakPointerToQObject);
            F(TrackingPointerToQObject);
            F(IsGadget);
#undef F

            return l.join(QStringLiteral(", "));
        }
        case 5: {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            auto mt = QMetaType(metaTypeId);
            return mt.isEqualityComparable() && mt.isOrdered();
#else
            return QMetaType::hasRegisteredComparators(metaTypeId);
#endif
        }
        case 6:
            return QMetaType::hasRegisteredDebugStreamOperator(metaTypeId);
        }
    } else if (role == MetaTypeRoles::MetaObjectIdRole && index.column() == 0) {
        if (auto mo = QMetaType::metaObjectForType(metaTypeId))
            return QVariant::fromValue(ObjectId(const_cast<QMetaObject *>(mo), "const QMetaObject*"));
    }

    return QVariant();
}

int MetaTypesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_metaTypes.size();
}

int MetaTypesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 7;
}

void MetaTypesModel::scanMetaTypes()
{
    QVector<int> metaTypes;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    for (int mtId = 0; mtId <= QMetaType::User; ++mtId) {
        if (!MetaObjectRegistry::isTypeIdRegistered(mtId))
            continue;
        const auto name = QMetaType::typeName(mtId);
        if (strstr(name, "GammaRay::") != name)
            metaTypes.push_back(mtId);
    }
    for (int mtId = QMetaType::User + 1; QMetaType::isRegistered(mtId); ++mtId) {
        const auto name = QMetaType::typeName(mtId);
        if (strstr(name, "GammaRay::") != name)
            metaTypes.push_back(mtId);
    }
#else
    for (int mtId = 0; mtId <= QMetaType::User || QMetaType::isRegistered(mtId); ++mtId) {
        if (!QMetaType::isRegistered(mtId))
            continue;
        const auto name = QMetaType::typeName(mtId);
        if (strstr(name, "GammaRay::") != name)
            metaTypes.push_back(mtId);
    }
#endif

    auto itOld = m_metaTypes.constBegin();
    auto itNew = metaTypes.constBegin();

    for (; itOld != m_metaTypes.constEnd() && itNew != metaTypes.constEnd(); ++itOld, ++itNew) {
        if (*itOld != *itNew)
            break;
    }

    if (itOld != m_metaTypes.constEnd()) {
        const auto row = std::distance(m_metaTypes.constBegin(), itOld);
        beginRemoveRows(QModelIndex(), row, m_metaTypes.size() - 1);
        m_metaTypes.remove(row, m_metaTypes.size() - row);
        endRemoveRows();
    }

    if (itNew != metaTypes.constEnd()) {
        const auto count = std::distance(itNew, metaTypes.constEnd());
        beginInsertRows(QModelIndex(), m_metaTypes.size(), m_metaTypes.size() + count - 1);
        std::copy(itNew, metaTypes.constEnd(), std::back_inserter(m_metaTypes));
        endInsertRows();
    }
}
