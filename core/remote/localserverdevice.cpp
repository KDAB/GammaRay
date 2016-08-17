/*
  localserverdevice.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "localserverdevice.h"

#include <QLocalServer>
#include <QLocalSocket>

using namespace GammaRay;

LocalServerDevice::LocalServerDevice(QObject *parent)
    : ServerDeviceImpl<QLocalServer>(parent)
{
    m_server = new QLocalServer(this);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_server->setSocketOptions(QLocalServer::WorldAccessOption);
#endif
    connect(m_server, SIGNAL(newConnection()), this, SIGNAL(newConnection()));
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
