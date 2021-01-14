/*
  outboundconnectionmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "outboundconnectionsmodel.h"
#include "core/probe.h"

#include <private/qobject_p.h>

using namespace GammaRay;

OutboundConnectionsModel::OutboundConnectionsModel(QObject *parent)
    : AbstractConnectionsModel(parent)
{
}

OutboundConnectionsModel::~OutboundConnectionsModel() = default;

void OutboundConnectionsModel::setObject(QObject *object)
{
    clear();
    m_object = object;
    if (!object) {
        return;
    }
    setConnections(outboundConnectionsForObject(object));
}

QVector<AbstractConnectionsModel::Connection> OutboundConnectionsModel::outboundConnectionsForObject(QObject *object)
{
    QVector<Connection> connections;
    QObjectPrivate *d = QObjectPrivate::get(object);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QObjectPrivate::ConnectionData *cd = d->connections.loadRelaxed();
    if (!cd)
        return connections;

    auto cl = cd->signalVector.loadRelaxed();
#else
    if (!d->connectionLists)
        return connections;

    // HACK: the declaration of d->connectionsLists is not accessible for us...
    const auto cl = reinterpret_cast<QVector<QObjectPrivate::ConnectionList> *>(d->connectionLists);
#endif
    if (!cl)
        return connections;

    for (int signalIndex = 0; signalIndex < cl->count(); ++signalIndex) {
        const QObjectPrivate::Connection *c = cl->at(signalIndex).first;
        while (c) {
            if (!c->receiver || Probe::instance()->filterObject(c->receiver)) {
                c = c->nextConnectionList;
                continue;
            }

            Connection conn;
            conn.endpoint = c->receiver;
            conn.signalIndex = signalIndexToMethodIndex(object, signalIndex);
            if (c->isSlotObject)
                conn.slotIndex = -1;
            else
            conn.slotIndex = c->method();
            conn.type = c->connectionType;
            c = c->nextConnectionList;
            connections.push_back(conn);
        }
    }

    return connections;
}

QVariant OutboundConnectionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_object)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const Connection &conn = m_connections.at(index.row());
        switch (index.column()) {
        case 0:
            return displayString(m_object, conn.signalIndex);
        case 1:
            return displayString(conn.endpoint);
        case 2:
            if (conn.slotIndex < 0) {
                return tr("<slot object>");
            }
            return displayString(conn.endpoint, conn.slotIndex);
        }
    }

    return AbstractConnectionsModel::data(index, role);
}

QVariant OutboundConnectionsModel::headerData(int section, Qt::Orientation orientation,
                                              int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Signal");
        case 1:
            return tr("Receiver");
        case 2:
            return tr("Slot");
        }
    }
    return AbstractConnectionsModel::headerData(section, orientation, role);
}
