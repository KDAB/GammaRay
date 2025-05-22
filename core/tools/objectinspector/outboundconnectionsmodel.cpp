/*
  outboundconnectionsmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>
#include "outboundconnectionsmodel.h"
#include "core/probe.h"

#include <private/qobject_p.h>
#include <private/qobject_p_p.h>

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
    QObjectPrivate::ConnectionData *cd = d->connections.loadRelaxed();
    if (!cd)
        return connections;

    auto cl = cd->signalVector.loadRelaxed();
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
