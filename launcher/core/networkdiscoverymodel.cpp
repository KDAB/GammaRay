/*
  networkdiscoverymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "networkdiscoverymodel.h"

#include <common/endpoint.h>

#include <QDataStream>
#include <QUdpSocket>
#include <QTimer>
#include <QNetworkDatagram>

#include <algorithm>

using namespace GammaRay;

bool NetworkDiscoveryModel::ServerInfo::operator==(const NetworkDiscoveryModel::ServerInfo &other) const
{
    return url == other.url;
}

NetworkDiscoveryModel::NetworkDiscoveryModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_socket(new QUdpSocket(this))
{
    m_socket->bind(Endpoint::broadcastPort(), QUdpSocket::ShareAddress);
    connect(m_socket, &QIODevice::readyRead, this, &NetworkDiscoveryModel::processPendingDatagrams);

    auto *expireTimer = new QTimer(this);
    expireTimer->setInterval(15 * 1000);
    expireTimer->setSingleShot(false);
    connect(expireTimer, &QTimer::timeout, this, &NetworkDiscoveryModel::expireEntries);
    expireTimer->start();
}

NetworkDiscoveryModel::~NetworkDiscoveryModel() = default;

void NetworkDiscoveryModel::processPendingDatagrams()
{
    while (m_socket->hasPendingDatagrams()) {
        const auto datagram = m_socket->receiveDatagram();
        const auto buffer = datagram.data();
        const auto senderAddr = datagram.senderAddress();

        QDataStream stream(buffer);
        qint32 broadcastVersion;
        stream >> broadcastVersion;
        if (broadcastVersion != Protocol::broadcastFormatVersion())
            continue;

        ServerInfo info;
        stream >> info.version >> info.url >> info.label;
        info.lastSeen = QDateTime::currentDateTime();
        // sender address is more reliable in case the probe runs on
        // a system with multiple public network interfaces
        if (!senderAddr.isNull() && info.url.scheme() == QLatin1String("tcp")) {
            switch (senderAddr.protocol()) {
            case QAbstractSocket::IPv4Protocol:
                info.url.setHost(senderAddr.toString());
                break;
            case QAbstractSocket::IPv6Protocol:
                info.url.setHost(QLatin1Char('[') + senderAddr.toString() + QLatin1Char(']'));
                break;
            default:
                break;
            }
        }

        auto it = std::find(m_data.begin(), m_data.end(), info);
        if (it == m_data.end()) {
            beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
            m_data.push_back(info);
            endInsertRows();
        } else {
            it->lastSeen = info.lastSeen;
        }
    }
}

void NetworkDiscoveryModel::expireEntries()
{
    const QDateTime threshold = QDateTime::currentDateTime().addSecs(-30);
    for (auto it = m_data.begin(); it != m_data.end();) {
        if (it->lastSeen >= threshold) {
            ++it;
        } else {
            const int currentRow = std::distance(m_data.begin(), it);
            beginRemoveRows(QModelIndex(), currentRow, currentRow);
            it = m_data.erase(it);
            endRemoveRows();
        }
    }
}

QVariant NetworkDiscoveryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const ServerInfo &info = m_data.at(index.row());
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return info.label;
        case 1:
            return info.url.toString();
        }
    } else if (role == Qt::ToolTipRole) {
        if (info.version != Protocol::version())
            return tr("Incompatible GammaRay version.");
    } else if (role == HostNameRole) {
        return info.url.host();
    } else if (role == PortRole) {
        return info.url.port();
    } else if (role == CompatibleRole) {
        return info.version == Protocol::version();
    } else if (role == UrlStringRole) {
        return info.url.toString();
    }

    return QVariant();
}

int NetworkDiscoveryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int NetworkDiscoveryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.size();
}

QVariant NetworkDiscoveryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return tr("Name");
        case 1:
            return tr("Host");
        }
    }
    return QVariant();
}

Qt::ItemFlags NetworkDiscoveryModel::flags(const QModelIndex &index) const
{
    const Qt::ItemFlags baseFlags = QAbstractTableModel::flags(index);
    if (!index.isValid())
        return baseFlags;

    const ServerInfo &info = m_data.at(index.row());
    if (info.version != Protocol::version())
        return baseFlags & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    return baseFlags;
}
