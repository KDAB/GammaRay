/*
  clientdevice.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "clientdevice.h"
#include "tcpclientdevice.h"
#include "localclientdevice.h"

#include <QDebug>

using namespace GammaRay;

ClientDevice::ClientDevice(QObject *parent)
    : QObject(parent)
    , m_tries(0)
{
}

ClientDevice::~ClientDevice() = default;

ClientDevice *ClientDevice::create(const QUrl &url, QObject *parent)
{
    ClientDevice *device = nullptr;
    if (url.scheme() == QLatin1String("tcp"))
        device = new TcpClientDevice(parent);
    else if (url.scheme() == QLatin1String("local"))
        device = new LocalClientDevice(parent);

    if (!device) {
        qWarning() << "Unsupported transport protocol:" << url.toString();
        return nullptr;
    }

    device->m_serverAddress = url;
    return device;
}

void ClientDevice::setTryAgain(int tries)
{
    m_tries = tries;
}
