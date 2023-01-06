/*
  serverdevice.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
