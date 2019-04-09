/*
  networkconfigurationmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "networkconfigurationmodel.h"

#include <core/varianthandler.h>

#include <QNetworkConfigurationManager>
#include <QTimer>

using namespace GammaRay;

NetworkConfigurationModel::NetworkConfigurationModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_mgr(nullptr)
{
}

NetworkConfigurationModel::~NetworkConfigurationModel() = default;

int NetworkConfigurationModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 8;
}

int NetworkConfigurationModel::rowCount(const QModelIndex& parent) const
{
    // delayed lazy loading, not just for performance, creating QNetworkConfigurationManager
    // has tons of side-effects such as D-Bus communication on Linux, that will possibly
    // fail when holding the Probe::objectLock
    if (!m_mgr) {
        QTimer::singleShot(0, this, &NetworkConfigurationModel::init);
        return 0;
    }

    if (parent.isValid())
        return 0;
    return m_configs.size();
}

Qt::ItemFlags NetworkConfigurationModel::flags(const QModelIndex& index) const
{
    const auto baseFlags = QAbstractTableModel::flags(index);
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    if (index.column() == 3)
        return baseFlags | Qt::ItemIsEditable;
#endif
    return baseFlags;
}

QVariant NetworkConfigurationModel::data(const QModelIndex& index, int role) const
{
    if (!m_mgr || !index.isValid())
        return QVariant();

    const auto conf = m_configs.at(index.row());
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return conf.name();
            case 1:
                return conf.identifier();
            case 2:
                return conf.bearerTypeName();
            case 3:
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
                return conf.connectTimeout();
#else
                return QStringLiteral("?");
#endif
            case 4:
                return conf.isRoamingAvailable();
            case 5:
                return VariantHandler::displayString(QVariant::fromValue(conf.purpose()));
            case 6:
                return VariantHandler::displayString(QVariant::fromValue(conf.state()));
            case 7:
                return VariantHandler::displayString(QVariant::fromValue(conf.type()));
        }
    } else if (role == Qt::EditRole && index.column() == 3) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
        return conf.connectTimeout();
#endif
    } else if (role == NetworkConfigurationModelRoles::DefaultConfigRole && index.column() == 0) {
        if (conf == m_mgr->defaultConfiguration())
            return true;
    }

    return QVariant();
}

bool NetworkConfigurationModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!m_mgr || !index.isValid() || role != Qt::EditRole || index.column() != 3 || value.isNull())
        return false;

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    auto conf = m_configs[index.row()];
    conf.setConnectTimeout(value.toInt());
#endif
    emit dataChanged(index, index);
    return true;
}

void NetworkConfigurationModel::init()
{
    if (m_mgr)
        return;

    beginResetModel();
    m_mgr = new QNetworkConfigurationManager(this);
    const auto configs = m_mgr->allConfigurations();
    m_configs.reserve(configs.size());
    std::copy(configs.begin(), configs.end(), std::back_inserter(m_configs));

    connect(m_mgr, &QNetworkConfigurationManager::configurationAdded, this, &NetworkConfigurationModel::configurationAdded);
    connect(m_mgr, &QNetworkConfigurationManager::configurationChanged, this, &NetworkConfigurationModel::configurationChanged);
    connect(m_mgr, &QNetworkConfigurationManager::configurationRemoved, this, &NetworkConfigurationModel::configurationRemoved);
    endResetModel();
}

void NetworkConfigurationModel::configurationAdded(const QNetworkConfiguration& config)
{
    const auto it = std::find(m_configs.begin(), m_configs.end(), config);
    if (it != m_configs.end())
        return;
    beginInsertRows(QModelIndex(), m_configs.size(), m_configs.size());
    m_configs.push_back(config);
    endInsertRows();
}

void NetworkConfigurationModel::configurationChanged(const QNetworkConfiguration& config)
{
    const auto it = std::find(m_configs.begin(), m_configs.end(), config);
    if (it == m_configs.end())
        return;
    const auto idx = std::distance(m_configs.begin(), it);
    emit dataChanged(index(idx, 0), index(idx, columnCount() - 1));
}

void NetworkConfigurationModel::configurationRemoved(const QNetworkConfiguration& config)
{
    const auto it = std::find(m_configs.begin(), m_configs.end(), config);
    if (it == m_configs.end())
        return;
    const auto idx = std::distance(m_configs.begin(), it);
    beginRemoveRows(QModelIndex(), idx, idx);
    m_configs.erase(it);
    endRemoveRows();
}
