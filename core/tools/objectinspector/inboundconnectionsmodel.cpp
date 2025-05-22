/*
  inboundconnectionsmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>
#include "inboundconnectionsmodel.h"
#include "core/probe.h"

#include <private/qobject_p.h>
#include <private/qobject_p_p.h>

using namespace GammaRay;

InboundConnectionsModel::InboundConnectionsModel(QObject *parent)
    : AbstractConnectionsModel(parent)
{
}

InboundConnectionsModel::~InboundConnectionsModel() = default;

void InboundConnectionsModel::setObject(QObject *object)
{
    clear();
    m_object = object;
    if (!object)
        return;

    setConnections(inboundConnectionsForObject(object));
}

QVector<AbstractConnectionsModel::Connection> InboundConnectionsModel::inboundConnectionsForObject(QObject *object)
{
    QVector<Connection> connections;
    QObjectPrivate *d = QObjectPrivate::get(object);
    QObjectPrivate::ConnectionData *cd = d->connections.loadRelaxed();
    if (cd && cd->senders) {
        auto *senders = cd->senders;
        for (QObjectPrivate::Connection *s = senders; s; s = s->next) {
            if (!s->sender || Probe::instance()->filterObject(s->sender))
                continue;

            Connection conn;
            conn.endpoint = s->sender;
            conn.signalIndex = signalIndexToMethodIndex(s->sender, s->signal_index);
            if (s->isSlotObject) {
                conn.slotIndex = -1;
            } else {
                conn.slotIndex = s->method();
            }
            conn.type = s->connectionType;
            connections.push_back(conn);
        }
    }

    return connections;
}

QVariant InboundConnectionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_object)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const Connection &conn = m_connections.at(index.row());
        switch (index.column()) {
        case 0:
            return displayString(conn.endpoint);
        case 1:
            return displayString(conn.endpoint, conn.signalIndex);
        case 2:
            if (conn.slotIndex < 0) {
                return tr("<slot object context>");
            }
            return displayString(m_object, conn.slotIndex);
        }
    }

    return AbstractConnectionsModel::data(index, role);
}

QVariant InboundConnectionsModel::headerData(int section, Qt::Orientation orientation,
                                             int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Sender");
        case 1:
            return tr("Signal");
        case 2:
            return tr("Slot");
        }
    }
    return AbstractConnectionsModel::headerData(section, orientation, role);
}
