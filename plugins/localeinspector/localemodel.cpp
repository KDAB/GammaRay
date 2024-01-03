/*
  localemodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "localemodel.h"
#include "localedataaccessor.h"

#include <QDebug>
#include <QLocale>

using namespace GammaRay;

LocaleModel::LocaleModel(LocaleDataAccessorRegistry *registry, QObject *parent)
    : QAbstractTableModel(parent)
    , m_registry(registry)
{
    init();
    connect(registry, &LocaleDataAccessorRegistry::accessorAdded, this, &LocaleModel::accessorAdded);
    connect(registry, &LocaleDataAccessorRegistry::accessorRemoved, this, &LocaleModel::accessorRemoved);
}

int LocaleModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_localeData.size();
}

QVariant LocaleModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()
        || index.row() >= m_locales.size()
        || index.column() >= m_localeData.size())
        return QVariant();

    const QLocale l = m_locales.at(index.row());
    return m_localeData.at(index.column())->data(l, role);
}

QVariant LocaleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QAbstractItemModel::headerData(section, orientation, role);
    if (orientation == Qt::Vertical)
        return QAbstractItemModel::headerData(section, orientation, role);
    LocaleDataAccessor *d = m_localeData.at(section);
    return d->accessorName();
}

void LocaleModel::init()
{
    m_localeData = m_registry->enabledAccessors();

    m_locales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript,
                                         QLocale::AnyCountry)
                    .toVector();
}

void LocaleModel::accessorAdded()
{
    Q_ASSERT(m_localeData.size() + 1 == m_registry->enabledAccessors().size());
    beginInsertColumns(QModelIndex(), m_localeData.size(), m_localeData.size());
    m_localeData = m_registry->enabledAccessors();
    endInsertColumns();
}

void LocaleModel::accessorRemoved(int idx)
{
    Q_ASSERT(m_localeData.size() - 1 == m_registry->enabledAccessors().size());
    beginRemoveColumns(QModelIndex(), idx, idx);
    m_localeData = m_registry->enabledAccessors();
    endRemoveColumns();
}

int LocaleModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_locales.size();
}
