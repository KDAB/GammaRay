/*
  clientsmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "clientsmodel.h"

#include <common/objectid.h>

#include <QFile>
#include <QTextStream>
#include <QWaylandClient>

using namespace GammaRay;

ClientsModel::ClientsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

ClientsModel::~ClientsModel() = default;

QWaylandClient *ClientsModel::client(int index) const
{
    return m_clients.at(index);
}

void ClientsModel::addClient(QWaylandClient *client)
{
    beginInsertRows(QModelIndex(), m_clients.count(), m_clients.count());
    m_clients.append(client);
    endInsertRows();
}

void ClientsModel::removeClient(QWaylandClient *client)
{
    int index = -1;
    for (int i = 0; i < m_clients.count(); ++i) {
        if (m_clients.at(i) == client) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_clients.removeAt(index);
    endRemoveRows();
}

int ClientsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_clients.count();
}

int ClientsModel::columnCount(const QModelIndex &) const
{
    return EndColumn;
}

QVariant ClientsModel::data(const QModelIndex &index, int role) const
{
    auto client = m_clients.at(index.row());

    switch (index.column()) {
    case PidColumn:
        if (role == Qt::DisplayRole)
            return client->processId();
        if (role == ObjectIdRole)
            return QVariant::fromValue(ObjectId(client));
        break;
    case CommandColumn: {
        if (role != Qt::DisplayRole)
            return QVariant();
        auto pid = client->processId();
        QByteArray path;
        QTextStream(&path) << "/proc/" << pid << "/cmdline";
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            return QStringLiteral("Not available :/");
        }

        QByteArray data = file.readAll();
        data.replace('\0', ' ');
        return data;
    }
    }
    return QVariant();
}

QMap<int, QVariant> ClientsModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> map;
    auto insertRole = [&](int role) {
        map[role] = data(index, role);
    };
    insertRole(Qt::DisplayRole);
    insertRole(ObjectIdRole);
    return map;
}

QVariant ClientsModel::headerData(int section, Qt::Orientation orientation, int) const
{
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case PidColumn:
            return QStringLiteral("pid");
        case CommandColumn:
            return QStringLiteral("command");
        }
    }
    return QString::number(section + 1);
}
