/*
  tcpserverdevice.cpp

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

#include "tcpserverdevice.h"
#include "server.h"

#include <QHostAddress>
#include <QNetworkInterface>
#include <QUdpSocket>

using namespace GammaRay;

TcpServerDevice::TcpServerDevice(QObject *parent)
    : ServerDeviceImpl<QTcpServer>(parent)
    , m_broadcastSocket(new QUdpSocket(this))
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &ServerDevice::newConnection);
}

TcpServerDevice::~TcpServerDevice() = default;

bool TcpServerDevice::listen()
{
    const QHostAddress address(m_address.host());
    // try the requested port first, and fall back to a random port otherwise
    auto result = m_server->listen(address, m_address.port());
    if (!result) {
        result = m_server->listen(address, 0);
    }
    emit externalAddressChanged();
    return result;
}

bool TcpServerDevice::isListening() const
{
    return m_server->isListening();
}

QString TcpServerDevice::bestAvailableIP(const QHostAddress &address) const
{
    QString firstHostFound;
    foreach (const QNetworkInterface &inter, QNetworkInterface::allInterfaces()) {
        if (!(inter.flags() & QNetworkInterface::IsUp)
            || !(inter.flags() & QNetworkInterface::IsRunning)
            || (inter.flags() & QNetworkInterface::IsLoopBack))
            continue;

        foreach (const QNetworkAddressEntry &addrEntry, inter.addressEntries()) {
            const QHostAddress addr = addrEntry.ip();

            // Return the ip according to the listening server protocol.
            if (addr.protocol() != m_server->serverAddress().protocol()
                || !addr.scopeId().isEmpty())
                continue;

            //If its our desired IP (e.g. from --listen) return early
            if (addr == address)
                return addr.toString();
            if (firstHostFound.isEmpty())
                firstHostFound = addr.toString();
        }
    }
    return firstHostFound;
}

QUrl TcpServerDevice::externalAddress() const
{
    const QHostAddress address(m_server->serverAddress());
    QString myHost;

    if (address.isLoopback()) {
        myHost = address.toString();
    } else {
        //scan Interfaces for available IPs, use requested address if we can find it.
        myHost = bestAvailableIP(address);
    }

    // if localhost is all we got, use that rather than nothing
    if (myHost.isEmpty()) {
        switch (m_server->serverAddress().protocol()) {
        case QAbstractSocket::IPv4Protocol:
        case QAbstractSocket::AnyIPProtocol:
            myHost = QHostAddress(QHostAddress::LocalHost).toString();
            break;
        case QAbstractSocket::IPv6Protocol:
            myHost = QHostAddress(QHostAddress::LocalHostIPv6).toString();
            break;
        case QAbstractSocket::UnknownNetworkLayerProtocol:
            qWarning() << "TcpServerDevice::externalAddress - unknown TCP protocol";
            return m_address;
        }
    }

    QUrl url;
    url.setScheme(QStringLiteral("tcp"));
    url.setHost(myHost);
    url.setPort(m_server->serverPort());
    return url;
}

void TcpServerDevice::broadcast(const QByteArray &data)
{
    const QHostAddress address = m_server->serverAddress();

    // broadcast announcement only if we are actually listinging to remote connections
    if (address.isLoopback())
        return;

    m_broadcastSocket->writeDatagram(data.data(),
                                     data.size(), QHostAddress::Broadcast, Server::broadcastPort());
}
