/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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
                                         QLocale::AnyCountry).toVector();
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
