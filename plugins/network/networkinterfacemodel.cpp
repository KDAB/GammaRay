/*
  networkinterfacemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "networkinterfacemodel.h"

#include <core/metaenum.h>

#include <QNetworkInterface>

#include <limits>

using namespace GammaRay;

#define F(x) { QNetworkInterface:: x, #x }
static const MetaEnum::Value<QNetworkInterface::InterfaceFlag> interface_flag_table[] = {
    F(IsUp),
    F(IsRunning),
    F(CanBroadcast),
    F(IsLoopBack),
    F(IsPointToPoint),
    F(CanMulticast)
};
#undef F

static const quintptr IFACE_ID = std::numeric_limits<quintptr>::max();

NetworkInterfaceModel::NetworkInterfaceModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    m_interfaces = QNetworkInterface::allInterfaces();
}

NetworkInterfaceModel::~NetworkInterfaceModel() = default;

int NetworkInterfaceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

int NetworkInterfaceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        if (parent.internalId() == IFACE_ID)
            return m_interfaces.at(parent.row()).addressEntries().size();
        return 0;
    }
    return m_interfaces.size();
}

QVariant NetworkInterfaceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if (index.internalId() == IFACE_ID) {
            const auto &iface = m_interfaces.at(index.row());
            switch (index.column()) {
            case 0:
                if (iface.name() == iface.humanReadableName())
                    return iface.name();
                return QStringLiteral("%1 (%2)").arg(iface.name(), iface.humanReadableName());
            case 1:
                return iface.hardwareAddress();
            case 2:
                return MetaEnum::flagsToString(iface.flags(), interface_flag_table);
            }
        } else if (index.column() == 0) {
            const auto &iface = m_interfaces.at(index.internalId());
            const auto &addr = iface.addressEntries().at(index.row());
            return QString(addr.ip().toString() + QLatin1Char('/') + addr.netmask().toString());
        }
    }

    return QVariant();
}

QVariant NetworkInterfaceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Interface");
        case 1:
            return tr("Hardware Address");
        case 2:
            return tr("Flags");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex NetworkInterfaceModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || child.internalId() == IFACE_ID)
        return {};
    return createIndex(child.internalId(), 0, IFACE_ID);
}

QModelIndex NetworkInterfaceModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return createIndex(row, column, IFACE_ID);
    if (parent.internalId() == IFACE_ID)
        return createIndex(row, column, parent.row());
    return {};
}
