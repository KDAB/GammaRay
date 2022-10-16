/*
  localeaccessormodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "localeaccessormodel.h"
#include "localedataaccessor.h"

#include <qmath.h>

#include <QDebug>

using namespace GammaRay;

LocaleAccessorModel::LocaleAccessorModel(LocaleDataAccessorRegistry *registry, QObject *parent)
    : QAbstractTableModel(parent)
    , m_registry(registry)
{
}

int LocaleAccessorModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    int area = m_registry->accessors().size();
    return qSqrt(area);
}

int LocaleAccessorModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    int area = m_registry->accessors().size();
    return qCeil(( float )area / ( int )qSqrt(area));
}

Qt::ItemFlags LocaleAccessorModel::flags(const QModelIndex &index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant LocaleAccessorModel::data(const QModelIndex &index, int role) const
{
    QVector<LocaleDataAccessor *> acc = m_registry->accessors();
    int offset = (index.row() * columnCount()) + index.column();
    if (offset >= acc.size())
        return QVariant();

    LocaleDataAccessor *accessor = acc.at(offset);
    switch (role) {
    case Qt::DisplayRole:
        return accessor->accessorName();
    case Qt::CheckStateRole:
        return m_registry->enabledAccessors().contains(accessor)
            ? Qt::Checked
            : Qt::Unchecked;
    case AccessorRole:
        return QVariant::fromValue<LocaleDataAccessor *>(accessor);
    default:
        return QVariant();
    }
}

bool LocaleAccessorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::CheckStateRole)
        return QAbstractItemModel::setData(index, value, role);
    bool enabled = value.toInt() == Qt::Checked;
    LocaleDataAccessor *accessor = index.data(AccessorRole).value<LocaleDataAccessor *>();
    m_registry->setAccessorEnabled(accessor, enabled);
    emit dataChanged(index, index);
    return true;
}
