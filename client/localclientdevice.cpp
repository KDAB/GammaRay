/*
  localclientdevice.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "localclientdevice.h"

using namespace GammaRay;

LocalClientDevice::LocalClientDevice(QObject *parent)
    : ClientDeviceImpl<QLocalSocket>(parent)
{
    m_socket = new QLocalSocket(this);
    connect(m_socket, &QLocalSocket::connected, this, &ClientDevice::connected);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    connect(m_socket, static_cast<void (QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
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
