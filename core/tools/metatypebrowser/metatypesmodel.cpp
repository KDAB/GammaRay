/*
  metatypesmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
            QString name(QMetaType(metaTypeId).name());
            if (name.isEmpty())
                return tr("N/A");
            return name;
        }
        case 1:
            return metaTypeId;
        case 2:
            return QMetaType(metaTypeId).sizeOf();
        case 3:
            return Util::addressToString(QMetaType(metaTypeId).metaObject());
        case 4: {
            const QMetaType::TypeFlags flags = QMetaType(metaTypeId).flags();
            QStringList l;
#define F(x)                  \
    if (flags & QMetaType::x) \
    l.push_back(QStringLiteral(#x))
            F(NeedsConstruction);
            F(NeedsDestruction);
            F(RelocatableType);
            F(IsConst);
            F(IsQmlList);
            F(IsUnsignedEnumeration);
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
            auto mt = QMetaType(metaTypeId);
            return mt.isEqualityComparable() && mt.isOrdered();
        }
        case 6:
            return QMetaType(metaTypeId).hasRegisteredDebugStreamOperator();
        }
    } else if (role == MetaTypeRoles::MetaObjectIdRole && index.column() == 0) {
        if (auto mo = QMetaType(metaTypeId).metaObject())
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

    for (int mtId = 0; mtId <= QMetaType::User; ++mtId) {
        if (!MetaObjectRegistry::isTypeIdRegistered(mtId))
            continue;
        const auto name = QMetaType(mtId).name();
        if (strstr(name, "GammaRay::") != name)
            metaTypes.push_back(mtId);
    }
    for (int mtId = QMetaType::User + 1; QMetaType::isRegistered(mtId); ++mtId) {
        const auto name = QMetaType(mtId).name();
        if (strstr(name, "GammaRay::") != name)
            metaTypes.push_back(mtId);
    }

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
