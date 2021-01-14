/*
  serverdevice.cpp

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

#include "serverdevice.h"

#include "tcpserverdevice.h"
#include "localserverdevice.h"

#include <QDebug>
#include <QUrl>

using namespace GammaRay;

ServerDevice::ServerDevice(QObject *parent)
    : QObject(parent)
{
}

ServerDevice::~ServerDevice() = default;

void ServerDevice::setServerAddress(const QUrl &serverAddress)
{
    m_address = serverAddress;
}

void ServerDevice::broadcast(const QByteArray &data)
{
    Q_UNUSED(data);
}

ServerDevice *ServerDevice::create(const QUrl &serverAddress, QObject *parent)
{
    ServerDevice *device = nullptr;
    if (serverAddress.scheme() == QLatin1String("tcp"))
        device = new TcpServerDevice(parent);
    else if (serverAddress.scheme() == QLatin1String("local"))
        device = new LocalServerDevice(parent);

    if (!device) {
        qWarning() << "Unsupported transport protocol:" << serverAddress.toString();
        return nullptr;
    }
    device->setServerAddress(serverAddress);
    return device;
}
