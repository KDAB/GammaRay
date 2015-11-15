/*
  tcpserverdevice.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

TcpServerDevice::TcpServerDevice(QObject* parent):
    ServerDeviceImpl<QTcpServer>(parent),
    m_broadcastSocket(new QUdpSocket(this))
{
    m_server = new QTcpServer(this);
    connect(m_server, SIGNAL(newConnection()), this, SIGNAL(newConnection()));
}

TcpServerDevice::~TcpServerDevice()
{
}

bool TcpServerDevice::listen()
{
    const QHostAddress address(m_address.host());
    // try the requested port first, and fall back to a random port otherwise
    if (m_server->listen(address, m_address.port()))
        return true;
    return m_server->listen(address, 0);
}

QUrl TcpServerDevice::externalAddress() const
{
    QString myHost;
    foreach (const QHostAddress &addr, QNetworkInterface::allAddresses()) {
        if (addr == QHostAddress::LocalHost || addr == QHostAddress::LocalHostIPv6 || !addr.scopeId().isEmpty())
            continue;
        myHost = addr.toString();
        break;
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
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if (address.toString() == "127.0.0.1" || address.toString() == "::1")
#else
    if (address.isLoopback())
#endif
        return;

    m_broadcastSocket->writeDatagram(data.data(), data.size(), QHostAddress::Broadcast, Server::broadcastPort());
}
