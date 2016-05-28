/*
  networksupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "networksupport.h"
#include "networkinterfacemodel.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>

#include <common/metatypedeclarations.h> // FIXME move QHostAddress from there to here

#include <QLocalSocket>
#include <QNetworkProxy>
#include <QSocketNotifier>
#include <QTcpServer>

using namespace GammaRay;

Q_DECLARE_METATYPE(QAbstractSocket::PauseModes)
Q_DECLARE_METATYPE(QAbstractSocket::SocketType)
Q_DECLARE_METATYPE(QLocalSocket::LocalSocketError)
Q_DECLARE_METATYPE(QLocalSocket::LocalSocketState)
Q_DECLARE_METATYPE(QSocketNotifier::Type)


NetworkSupport::NetworkSupport(ProbeInterface *probe, QObject *parent) :
    QObject(parent)
{
    registerMetaTypes();

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.NetworkInterfaceModel"), new NetworkInterfaceModel(this));
}

NetworkSupport::~NetworkSupport()
{
}

void NetworkSupport::registerMetaTypes()
{
    MetaObject *mo = 0;
    MO_ADD_METAOBJECT1(QAbstractSocket, QIODevice);
    MO_ADD_PROPERTY_RO(QAbstractSocket, bool, isValid);
    MO_ADD_PROPERTY_RO(QAbstractSocket, quint16, localPort);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QHostAddress, localAddress);
    MO_ADD_PROPERTY_RO(QAbstractSocket, quint16, peerPort);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QHostAddress, peerAddress);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QString, peerName);
    MO_ADD_PROPERTY   (QAbstractSocket, qint64, readBufferSize, setReadBufferSize);
    MO_ADD_PROPERTY   (QAbstractSocket, QAbstractSocket::PauseModes, pauseMode, setPauseMode);
    MO_ADD_PROPERTY_RO(QAbstractSocket, qintptr, socketDescriptor);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketType, socketType);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketState, state);
    MO_ADD_PROPERTY_RO(QAbstractSocket, QAbstractSocket::SocketError, error);
#ifndef QT_NO_NETWORKPROXY
    MO_ADD_PROPERTY_RO(QAbstractSocket, QNetworkProxy, proxy);
#endif

    MO_ADD_METAOBJECT1(QLocalSocket, QIODevice);
    MO_ADD_PROPERTY_RO(QLocalSocket, QLocalSocket::LocalSocketError, error);
    MO_ADD_PROPERTY_RO(QLocalSocket, QString, fullServerName);
    MO_ADD_PROPERTY_RO(QLocalSocket, bool, isValid);
    MO_ADD_PROPERTY_RO(QLocalSocket, QString, serverName);
    MO_ADD_PROPERTY_RO(QLocalSocket, qintptr, socketDescriptor);
    MO_ADD_PROPERTY_RO(QLocalSocket, QLocalSocket::LocalSocketState, state);

    // FIXME: QAbstractSocket::setSocketOption() would be nice to have
    // FIXME: QQAbstractSocket::socketOption() would be nice to have

    MO_ADD_METAOBJECT1(QTcpServer, QObject);
    MO_ADD_PROPERTY_RO(QTcpServer, bool, isListening);
    MO_ADD_PROPERTY   (QTcpServer, int, maxPendingConnections, setMaxPendingConnections);
    MO_ADD_PROPERTY_RO(QTcpServer, quint16, serverPort);
    MO_ADD_PROPERTY_RO(QTcpServer, QHostAddress, serverAddress);
    MO_ADD_PROPERTY_RO(QTcpServer, qintptr, socketDescriptor);
    MO_ADD_PROPERTY_RO(QTcpServer, bool, hasPendingConnections);
    MO_ADD_PROPERTY_RO(QTcpServer, QAbstractSocket::SocketError, serverError);
    MO_ADD_PROPERTY_RO(QTcpServer, QString, errorString);
#ifndef QT_NO_NETWORKPROXY
    MO_ADD_PROPERTY_RO(QTcpServer, QNetworkProxy, proxy);
#endif

    MO_ADD_METAOBJECT1(QSocketNotifier, QObject);
    MO_ADD_PROPERTY_RO(QSocketNotifier, qintptr, socket);
    MO_ADD_PROPERTY_RO(QSocketNotifier, QSocketNotifier::Type, type);
    MO_ADD_PROPERTY   (QSocketNotifier, bool, isEnabled, setEnabled);
}


NetworkSupportFactory::NetworkSupportFactory(QObject *parent) :
    QObject(parent)
{
}

QString NetworkSupportFactory::name() const
{
    return tr("Network");
}
