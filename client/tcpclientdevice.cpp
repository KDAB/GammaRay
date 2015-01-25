/*
  tcpclientdevice.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "tcpclientdevice.h"

#include "client.h"

using namespace GammaRay;

TcpClientDevice::TcpClientDevice(QObject* parent) :
    ClientDeviceImpl<QTcpSocket>(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
}

void TcpClientDevice::connectToHost()
{
    m_socket->connectToHost(m_serverAddress.host(), m_serverAddress.port(Client::defaultPort()));
}

void TcpClientDevice::socketError()
{
    if (m_socket->error() == QAbstractSocket::ConnectionRefusedError)
        emit transientError();
    else
        emit persistentError(m_socket->errorString());
}
