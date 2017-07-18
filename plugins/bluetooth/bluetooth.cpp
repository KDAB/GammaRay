/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "bluetooth.h"

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>

#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothServer>
#include <QBluetoothServiceDiscoveryAgent>
#include <QBluetoothSocket>

#include <QDebug>

Q_DECLARE_METATYPE(QBluetooth::SecurityFlags)
Q_DECLARE_METATYPE(QBluetoothDeviceDiscoveryAgent::Error)
Q_DECLARE_METATYPE(QBluetoothDeviceDiscoveryAgent::InquiryType)
Q_DECLARE_METATYPE(QBluetoothServer::Error)
Q_DECLARE_METATYPE(QBluetoothServiceInfo::Protocol)
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(QBluetoothServiceDiscoveryAgent::Error)
Q_DECLARE_METATYPE(QBluetoothSocket::SocketError)
Q_DECLARE_METATYPE(QBluetoothSocket::SocketState)
#endif

using namespace GammaRay;

static QString bluetoothAddressToString(const QBluetoothAddress &addr)
{
    return addr.toString();
}

static QString bluetoothInquiryTypeToString(QBluetoothDeviceDiscoveryAgent::InquiryType type)
{
    switch (type) {
    case QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry:
        return QStringLiteral("Unlimited");
    case QBluetoothDeviceDiscoveryAgent::LimitedInquiry:
        return QStringLiteral("Limited");
    }
    Q_UNREACHABLE();
    return QString();
}

Bluetooth::Bluetooth(ProbeInterface *probe, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(probe);
    qRegisterMetaType<QBluetoothDeviceDiscoveryAgent::InquiryType>();

    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QBluetoothDeviceDiscoveryAgent, QObject);
    MO_ADD_PROPERTY_RO(QBluetoothDeviceDiscoveryAgent, QBluetoothDeviceDiscoveryAgent::Error,
                       error);
    MO_ADD_PROPERTY_RO(QBluetoothDeviceDiscoveryAgent, QString, errorString);
    MO_ADD_PROPERTY_RO(QBluetoothDeviceDiscoveryAgent, bool, isActive);

    MO_ADD_METAOBJECT1(QBluetoothLocalDevice, QObject);
    MO_ADD_PROPERTY_RO(QBluetoothLocalDevice, QBluetoothAddress, address);
    MO_ADD_PROPERTY_RO(QBluetoothLocalDevice, QList<QBluetoothAddress>, connectedDevices);
    MO_ADD_PROPERTY(QBluetoothLocalDevice, QBluetoothLocalDevice::HostMode, hostMode, setHostMode);
    MO_ADD_PROPERTY_RO(QBluetoothLocalDevice, bool, isValid);
    MO_ADD_PROPERTY_RO(QBluetoothLocalDevice, QString, name);

    MO_ADD_METAOBJECT1(QBluetoothServer, QObject);
    MO_ADD_PROPERTY_RO(QBluetoothServer, QBluetoothServer::Error, error);
    MO_ADD_PROPERTY_RO(QBluetoothServer, bool, hasPendingConnections);
    MO_ADD_PROPERTY_RO(QBluetoothServer, bool, isListening);
    MO_ADD_PROPERTY(QBluetoothServer, int, maxPendingConnections, setMaxPendingConnections);
    MO_ADD_PROPERTY(QBluetoothServer, QBluetooth::SecurityFlags, securityFlags, setSecurityFlags);
    MO_ADD_PROPERTY_RO(QBluetoothServer, quint16, serverPort);
    MO_ADD_PROPERTY_RO(QBluetoothServer, QBluetoothServiceInfo::Protocol, serverType);

    MO_ADD_METAOBJECT1(QBluetoothServiceDiscoveryAgent, QObject);
    MO_ADD_PROPERTY_RO(QBluetoothServiceDiscoveryAgent, QBluetoothServiceDiscoveryAgent::Error,
                       error);
    MO_ADD_PROPERTY_RO(QBluetoothServiceDiscoveryAgent, QString, errorString);
    MO_ADD_PROPERTY_RO(QBluetoothServiceDiscoveryAgent, bool, isActive);
    MO_ADD_PROPERTY_RO(QBluetoothServiceDiscoveryAgent, QBluetoothAddress, remoteAddress);
    MO_ADD_PROPERTY_CR(QBluetoothServiceDiscoveryAgent, QList<QBluetoothUuid>, uuidFilter,
                       setUuidFilter);

    MO_ADD_METAOBJECT1(QBluetoothSocket, QIODevice);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, QBluetoothSocket::SocketError, error);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, QString, errorString);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, QBluetoothAddress, localAddress);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, QString, localName);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, quint16, localPort);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, QBluetoothAddress, peerAddress);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, QString, peerName);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, quint16, peerPort);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, int, socketDescriptor);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, QBluetoothServiceInfo::Protocol, socketType);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, QBluetoothSocket::SocketState, state);

    VariantHandler::registerStringConverter<QBluetoothAddress>(bluetoothAddressToString);
    VariantHandler::registerStringConverter<QBluetoothDeviceDiscoveryAgent::InquiryType>(
        bluetoothInquiryTypeToString);
}
