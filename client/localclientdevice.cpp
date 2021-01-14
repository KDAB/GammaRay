/*
  localclientdevice.cpp

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

#include "localclientdevice.h"

using namespace GammaRay;

LocalClientDevice::LocalClientDevice(QObject *parent)
    : ClientDeviceImpl<QLocalSocket>(parent)
{
    m_socket = new QLocalSocket(this);
    connect(m_socket, &QLocalSocket::connected, this, &ClientDevice::connected);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    connect(m_socket, static_cast<void(QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
            this, &LocalClientDevice::socketError);
#else
    connect(m_socket, &QLocalSocket::errorOccurred, this, &LocalClientDevice::socketError);
#endif
}

void LocalClientDevice::connectToHost()
{
    m_socket->connectToServer(m_serverAddress.path());
}

void LocalClientDevice::disconnectFromHost()
{
    m_socket->disconnectFromServer();
}

void LocalClientDevice::socketError()
{
    switch (m_socket->error()) {
    case QLocalSocket::ConnectionRefusedError:
    case QLocalSocket::ServerNotFoundError:
    case QLocalSocket::SocketAccessError:
    case QLocalSocket::SocketTimeoutError:
    case QLocalSocket::ConnectionError:
    case QLocalSocket::UnknownSocketError:
        emit transientError();
        break;
    default:
        if (m_tries) {
            --m_tries;
            emit transientError();
        } else {
            emit persistentError(m_socket->errorString());
        }
        break;
    }
}
