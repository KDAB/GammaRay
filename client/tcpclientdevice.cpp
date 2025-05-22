/*
  tcpclientdevice.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "tcpclientdevice.h"

#include "client.h"

using namespace GammaRay;

TcpClientDevice::TcpClientDevice(QObject *parent)
    : ClientDeviceImpl<QTcpSocket>(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QAbstractSocket::connected, this, &ClientDevice::connected);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &TcpClientDevice::socketError);
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
