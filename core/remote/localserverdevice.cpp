/*
  localserverdevice.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "localserverdevice.h"

#include <QLocalServer>
#include <QLocalSocket>

using namespace GammaRay;

LocalServerDevice::LocalServerDevice(QObject *parent)
    : ServerDeviceImpl<QLocalServer>(parent)
{
    m_server = new QLocalServer(this);
    m_server->setSocketOptions(QLocalServer::WorldAccessOption);
    connect(m_server, &QLocalServer::newConnection, this, &ServerDevice::newConnection);
}

bool LocalServerDevice::listen()
{
    QLocalServer::removeServer(m_address.path());
    return m_server->listen(m_address.path());
}

bool LocalServerDevice::isListening() const
{
    return m_server->isListening();
}

QUrl LocalServerDevice::externalAddress() const
{
    return m_address;
}
