/*
  abstractconnectionsmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>
#include "abstractconnectionsmodel.h"

#include "common/tools/objectinspector/connectionsmodelroles.h"
#include "core/util.h"

#include <compat/qasconst.h>

#include <QMetaMethod>
#include <QStringList>

using namespace GammaRay;

AbstractConnectionsModel::AbstractConnectionsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

AbstractConnectionsModel::~AbstractConnectionsModel() = default;

int AbstractConnectionsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

int AbstractConnectionsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_connections.size();
}

QVariant AbstractConnectionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Connection &conn = m_connections.at(index.row());
    if (role == Qt::DisplayRole && index.column() == 3) {
        switch (conn.type) { // see qobject_p.h
        case 0:
            if (!conn.endpoint || !m_object)
                return tr("Auto");
            return tr("Auto (%1)").arg(conn.endpoint->thread() == m_object->thread() ? tr(
                                           "Direct") : tr("Queued"));
        case 1:
            return tr("Direct");
        case 2:
            return tr("Queued");
        case 3: // Qt5
        case 4: // Qt4
            return tr("Blocking");
        default:
            return tr("Unknown: %1").arg(conn.type);
        }
    }

    if (role == ConnectionsModelRoles::WarningFlagRole && index.column() == 0)
        return isDuplicate(conn) || isDirectCrossThreadConnection(conn);

    if (role == Qt::ToolTipRole) {
        QStringList tips;
        if (isDuplicate(conn))
            tips << tr(
                "Connections exists multiple times.\nThe connected slot is called multiple times when the signal is emitted.");

        if (isDirectCrossThreadConnection(conn))
            tips << tr(
                "Direct cross-thread connection.\nThe connected slot is called in the context of the emitting thread.");

        if (!tips.isEmpty())
            return tips.join(QStringLiteral("\n\n"));
    }

    if (role == ConnectionsModelRoles::EndpointRole)
        return QVariant::fromValue(conn.endpoint.data());

    if (role == ConnectionsModelRoles::ActionRole) {
        if (conn.endpoint && conn.endpoint != m_object)
            return ConnectionsModelActions::NavigateToEndpoint;
        return ConnectionsModelActions::NoAction;
    }

    return QVariant();
}

QVariant AbstractConnectionsModel::headerData(int section, Qt::Orientation orientation,
                                              int role) const
{
    if (section == 3 && orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return tr("Type");
    return QAbstractItemModel::headerData(section, orientation, role);
}

QString AbstractConnectionsModel::displayString(QObject *object, int methodIndex)
{
    if (!object)
        return tr("<destroyed>");
    if (methodIndex < 0)
        return tr("<unknown>");

    const QMetaMethod method = object->metaObject()->method(methodIndex);
    return Util::prettyMethodSignature(method);
}

QString AbstractConnectionsModel::displayString(QObject *object)
{
    if (!object)
        return tr("<destroyed>");
    return Util::displayString(object);
}

int AbstractConnectionsModel::signalIndexToMethodIndex(QObject *object, int signalIndex)
{
    if (signalIndex < 0)
        return signalIndex;
    Q_ASSERT(object);

    return Util::signalIndexToMethodIndex(object->metaObject(), signalIndex);
}

QMap< int, QVariant > AbstractConnectionsModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> d = QAbstractTableModel::itemData(index);
    d.insert(ConnectionsModelRoles::WarningFlagRole,
             data(index, ConnectionsModelRoles::WarningFlagRole));
    d.insert(ConnectionsModelRoles::ActionRole, data(index, ConnectionsModelRoles::ActionRole));
    return d;
}

bool AbstractConnectionsModel::isDuplicate(const QVector<Connection> &connections, const AbstractConnectionsModel::Connection& conn)
{
    for (const Connection &c : qAsConst(connections)) {
        if (&c == &conn)
            continue;
        if (c.endpoint == conn.endpoint
            && c.slotIndex >= 0 && c.slotIndex == conn.slotIndex
            && c.signalIndex >= 0 && c.signalIndex == conn.signalIndex)
            return true;
    }
    return false;
}

bool AbstractConnectionsModel::isDuplicate(const Connection &conn) const
{
    return isDuplicate(m_connections, conn);
}

bool AbstractConnectionsModel::isDirectCrossThreadConnection(QObject *object, const AbstractConnectionsModel::Connection &conn)
{
    if (!conn.endpoint || !object || conn.endpoint->thread() == object->thread())
        return false;
    return conn.type == 1; // direct
}


bool AbstractConnectionsModel::isDirectCrossThreadConnection(const Connection &conn) const
{
    return isDirectCrossThreadConnection(m_object, conn);
}

void AbstractConnectionsModel::clear()
{
    if (m_connections.isEmpty())
        return;

    beginRemoveRows(QModelIndex(), 0, m_connections.size() - 1);
    m_connections.clear();
    endRemoveRows();
}

void AbstractConnectionsModel::setConnections(const QVector<Connection> &connections)
{
    Q_ASSERT(m_connections.isEmpty());
    if (connections.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, connections.size() - 1);
    m_connections = connections;
    endInsertRows();
}
