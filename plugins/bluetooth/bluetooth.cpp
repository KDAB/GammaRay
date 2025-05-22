/*
  bluetooth.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
Q_DECLARE_METATYPE(QBluetoothServer::Error)
Q_DECLARE_METATYPE(QBluetoothServiceInfo::Protocol)

using namespace GammaRay;

Bluetooth::Bluetooth(Probe *probe, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(probe);

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
}
