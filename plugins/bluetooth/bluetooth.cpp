/*
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

#include <functional>

Q_DECLARE_METATYPE(QBluetooth::SecurityFlags)
Q_DECLARE_METATYPE(QBluetoothDeviceDiscoveryAgent::Error)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Q_DECLARE_METATYPE(QBluetoothDeviceDiscoveryAgent::InquiryType)
#endif
Q_DECLARE_METATYPE(QBluetoothServer::Error)
Q_DECLARE_METATYPE(QBluetoothServiceInfo::Protocol)

using namespace GammaRay;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
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
#endif

Bluetooth::Bluetooth(Probe *probe, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(probe);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qRegisterMetaType<QBluetoothDeviceDiscoveryAgent::InquiryType>();
#endif

    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QBluetoothDeviceDiscoveryAgent, QObject);
    MO_ADD_PROPERTY_RO(QBluetoothDeviceDiscoveryAgent, error);
    MO_ADD_PROPERTY_RO(QBluetoothDeviceDiscoveryAgent, errorString);
    MO_ADD_PROPERTY_RO(QBluetoothDeviceDiscoveryAgent, isActive);

    MO_ADD_METAOBJECT1(QBluetoothLocalDevice, QObject);
    MO_ADD_PROPERTY_RO(QBluetoothLocalDevice, address);
    MO_ADD_PROPERTY_RO(QBluetoothLocalDevice, connectedDevices);
    MO_ADD_PROPERTY(QBluetoothLocalDevice, hostMode, setHostMode);
    MO_ADD_PROPERTY_RO(QBluetoothLocalDevice, isValid);
    MO_ADD_PROPERTY_RO(QBluetoothLocalDevice, name);

    MO_ADD_METAOBJECT1(QBluetoothServer, QObject);
    MO_ADD_PROPERTY_RO(QBluetoothServer, error);
    MO_ADD_PROPERTY_RO(QBluetoothServer, hasPendingConnections);
    MO_ADD_PROPERTY_RO(QBluetoothServer, isListening);
    MO_ADD_PROPERTY(QBluetoothServer, maxPendingConnections, setMaxPendingConnections);
    MO_ADD_PROPERTY(QBluetoothServer, securityFlags, setSecurityFlags);
    MO_ADD_PROPERTY_RO(QBluetoothServer, serverPort);
    MO_ADD_PROPERTY_RO(QBluetoothServer, serverType);

    MO_ADD_METAOBJECT1(QBluetoothServiceDiscoveryAgent, QObject);
    MO_ADD_PROPERTY_RO(QBluetoothServiceDiscoveryAgent, error);
    MO_ADD_PROPERTY_RO(QBluetoothServiceDiscoveryAgent, errorString);
    MO_ADD_PROPERTY_RO(QBluetoothServiceDiscoveryAgent, isActive);
    MO_ADD_PROPERTY_RO(QBluetoothServiceDiscoveryAgent, remoteAddress);
    MO_ADD_PROPERTY_O2(QBluetoothServiceDiscoveryAgent, uuidFilter, setUuidFilter);

    MO_ADD_METAOBJECT1(QBluetoothSocket, QIODevice);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, error);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, errorString);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, localAddress);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, localName);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, localPort);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, peerAddress);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, peerName);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, peerPort);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, socketDescriptor);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, socketType);
    MO_ADD_PROPERTY_RO(QBluetoothSocket, state);

    VariantHandler::registerStringConverter<QBluetoothAddress>(std::mem_fn(&QBluetoothAddress::toString));
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    VariantHandler::registerStringConverter<QBluetoothDeviceDiscoveryAgent::InquiryType>(
        bluetoothInquiryTypeToString);
#endif
}
