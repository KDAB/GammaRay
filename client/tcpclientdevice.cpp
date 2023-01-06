/*
  tcpclientdevice.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "tcpclientdevice.h"

#include "client.h"

using namespace GammaRay;

TcpClientDevice::TcpClientDevice(QObject *parent)
    : ClientDeviceImpl<QTcpSocket>(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QAbstractSocket::connected, this, &ClientDevice::connected);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    connect(m_socket, static_cast<void (QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            this, &TcpClientDevice::socketError);
#else
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &TcpClientDevice::socketError);
#endif
}

void TcpClientDevice::connectToHost()
{
    m_socket->connectToHost(m_serverAddress.host(), m_serverAddress.port(Client::defaultPort()));
}

void TcpClientDevice::disconnectFromHost()
{
    m_socket->disconnectFromHost();
}

void TcpClientDevice::socketError()
{
    if (m_socket->error() == QAbstractSocket::ConnectionRefusedError)
        emit transientError();
    else
        emit persistentError(m_socket->errorString());
}
