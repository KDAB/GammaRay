/*
  client.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "client.h"
#include "clientdevice.h"
#include "messagestatisticsmodel.h"

#include <common/message.h>
#include <common/objectbroker.h>
#include <common/propertysyncer.h>

#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QUrl>

using namespace GammaRay;

Client::Client(QObject *parent)
    : Endpoint(parent)
    , m_clientDevice(0)
    , m_statModel(new MessageStatisticsModel)
    , m_initState(0)
{
    connect(this, SIGNAL(disconnected()), SLOT(socketDisconnected()));

    m_propertySyncer->setRequestInitialSync(true);

    ObjectBroker::registerModelInternal(QStringLiteral(
                                            "com.kdab.GammaRay.MessageStatisticsModel"),
                                        m_statModel);
}

Client::~Client()
{
}

Client *Client::instance()
{
    return static_cast<Client *>(s_instance);
}

bool Client::isRemoteClient() const
{
    return true;
}

QUrl Client::serverAddress() const
{
    return m_serverAddress;
}

void Client::connectToHost(const QUrl &url, int tryAgain)
{
    m_serverAddress = url;
    m_initState = 0;

    m_statModel->clear();
    m_clientDevice = ClientDevice::create(m_serverAddress, this);
    if (!m_clientDevice) {
        emit persisitentConnectionError(tr("Unsupported transport protocol."));
        return;
    }

    connect(m_clientDevice, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(m_clientDevice, SIGNAL(transientError()), this, SIGNAL(transientConnectionError()));
    connect(m_clientDevice, SIGNAL(persistentError(QString)), this,
            SIGNAL(persisitentConnectionError(QString)));
    connect(m_clientDevice, SIGNAL(transientError()), this, SLOT(resetClientDevice()));
    connect(m_clientDevice, SIGNAL(persistentError(QString)), this, SLOT(resetClientDevice()));
    m_clientDevice->setTryAgain(tryAgain);
    m_clientDevice->connectToHost();
}

void Client::disconnectFromHost()
{
    if (m_clientDevice) {
        m_clientDevice->disconnectFromHost();
        resetClientDevice();
    }
}

void Client::socketConnected()
{
    Q_ASSERT(m_clientDevice->device());
    setDevice(m_clientDevice->device());
}

void Client::resetClientDevice()
{
    if (m_clientDevice) {
        m_clientDevice->deleteLater();
        m_clientDevice = 0;
    }
}

void Client::socketDisconnected()
{
    foreach (const auto &objInfo, objectAddresses())
        removeObjectNameAddressMapping(objInfo.second);
    ObjectBroker::clear();
    resetClientDevice();
}

void Client::messageReceived(const Message &msg)
{
    m_statModel->addMessage(msg.address(), msg.type(), msg.size());
    // server version must be the very first message we get
    if (!(m_initState & VersionChecked)) {
        if (msg.address() != endpointAddress() || msg.type() != Protocol::ServerVersion) {
            emit persisitentConnectionError(tr(
                                                "Protocol violation, first message is not the server version."));
            disconnectFromHost();
        }
        qint32 serverVersion;
        msg >> serverVersion;
        if (serverVersion != Protocol::version()) {
            emit persisitentConnectionError(tr("Server version is %1, was expecting %2.").arg(
                                                serverVersion).arg(Protocol::version()));
            disconnectFromHost();
        }
        m_initState |= VersionChecked;
        return;
    }

    if (msg.address() == endpointAddress()) {
        switch (msg.type()) {
        case Protocol::ObjectAdded:
        {
            QString name;
            Protocol::ObjectAddress addr;
            msg >> name >> addr;
            addObjectNameAddressMapping(name, addr);
            m_statModel->addObject(addr, name);
            break;
        }
        case Protocol::ObjectRemoved:
        {
            QString name;
            msg >> name;
            removeObjectNameAddressMapping(name);
            break;
        }
        case Protocol::ObjectMapReply:
        {
            QVector<QPair<Protocol::ObjectAddress, QString> > objects;
            msg >> objects;
            for (QVector<QPair<Protocol::ObjectAddress, QString> >::const_iterator it =
                     objects.constBegin();
                 it != objects.constEnd(); ++it) {
                if (it->first != endpointAddress())
                    addObjectNameAddressMapping(it->second, it->first);
                m_statModel->addObject(it->first, it->second);
            }

            m_propertySyncer->setAddress(objectAddress(QStringLiteral(
                                                           "com.kdab.GammaRay.PropertySyncer")));
            Q_ASSERT(m_propertySyncer->address() != Protocol::InvalidObjectAddress);
            Endpoint::registerMessageHandler(
                m_propertySyncer->address(), m_propertySyncer, "handleMessage");

            m_initState |= ObjectMapReceived;
            break;
        }
        case Protocol::ServerInfo:
        {
            QString label;
            QString key;
            msg >> label >> key;
            setLabel(label);
            setKey(key);
            m_initState |= ServerInfoReceived;
            break;
        }
        default:
            qWarning() << Q_FUNC_INFO << "Got unhandled message:" << msg.type();
            return;
        }
        if (m_initState == InitComplete) {
            m_initState |= ConnectionEstablished;
            emit connectionEstablished();
        }
    } else {
        dispatchMessage(msg);
    }
}

Protocol::ObjectAddress Client::registerObject(const QString &name, QObject *object)
{
    Q_ASSERT(isConnected());
    Protocol::ObjectAddress address = Endpoint::registerObject(name, object);
    m_propertySyncer->addObject(address, object);
    m_propertySyncer->setObjectEnabled(address, true);

    monitorObject(address);
    return address;
}

void Client::registerMessageHandler(Protocol::ObjectAddress objectAddress, QObject *receiver,
                                    const char *messageHandlerName)
{
    Q_ASSERT(isConnected());
    Endpoint::registerMessageHandler(objectAddress, receiver, messageHandlerName);
    monitorObject(objectAddress);
}

void Client::unregisterMessageHandler(Protocol::ObjectAddress objectAddress)
{
    Endpoint::unregisterMessageHandler(objectAddress);
    unmonitorObject(objectAddress);
}

void Client::objectDestroyed(Protocol::ObjectAddress objectAddress, const QString & /*objectName*/,
                             QObject * /*object*/)
{
    unmonitorObject(objectAddress);
}

void Client::handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString & /*objectName*/)
{
    unmonitorObject(objectAddress);
}

void Client::monitorObject(Protocol::ObjectAddress objectAddress)
{
    if (!isConnected())
        return;
    Message msg(endpointAddress(), Protocol::ObjectMonitored);
    msg << objectAddress;
    send(msg);
}

void Client::unmonitorObject(Protocol::ObjectAddress objectAddress)
{
    if (!isConnected())
        return;
    Message msg(endpointAddress(), Protocol::ObjectUnmonitored);
    msg << objectAddress;
    send(msg);
}

void Client::doSendMessage(const GammaRay::Message &msg)
{
    m_statModel->addMessage(msg.address(), msg.type(), msg.size());
    Endpoint::doSendMessage(msg);
}
