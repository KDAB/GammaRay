/*
  server.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>
#include "server.h"
#include "serverdevice.h"
#include "probe.h"
#include "probesettings.h"
#include "multisignalmapper.h"

#include <common/protocol.h>
#include <common/message.h>
#include <common/propertysyncer.h>

#ifdef Q_OS_ANDROID
# include <QDir>
#endif

#include <QDebug>
#include <QTimer>
#include <QMetaMethod>

#include <iostream>

using namespace GammaRay;
using namespace std;

Server::Server(QObject *parent)
    : Endpoint(parent)
    , m_serverDevice(nullptr)
    , m_nextAddress(endpointAddress())
    , m_broadcastTimer(new QTimer(this))
    , m_signalMapper(new MultiSignalMapper(this))
{
    Message::resetNegotiatedDataVersion();

    if (!ProbeSettings::value(QStringLiteral("RemoteAccessEnabled"), true).toBool())
        return;

    m_serverDevice = ServerDevice::create(serverAddress(), this);
    if (!m_serverDevice)
        return;

    connect(m_serverDevice, &ServerDevice::newConnection, this, &Server::newConnection);

    m_broadcastTimer->setInterval(5 * 1000);
    m_broadcastTimer->setSingleShot(false);
    if (serverAddress().scheme() == QLatin1String("tcp")) {
        m_broadcastTimer->start();
    }
    connect(m_broadcastTimer, &QTimer::timeout, this, &Server::broadcast);
    connect(this, &Server::disconnected, m_broadcastTimer, [this]{ m_broadcastTimer->start(); });

    connect(m_signalMapper, &MultiSignalMapper::signalEmitted,
            this, &Server::forwardSignal);

    Endpoint::addObjectNameAddressMapping(QStringLiteral(
                                              "com.kdab.GammaRay.PropertySyncer"), ++m_nextAddress);
    m_propertySyncer->setAddress(m_nextAddress);
    Endpoint::registerObject(QStringLiteral("com.kdab.GammaRay.PropertySyncer"), m_propertySyncer);
    registerMessageHandler(m_nextAddress, m_propertySyncer, "handleMessage");
}

Server::~Server() = default;

bool Server::listen()
{
    Q_ASSERT(!m_serverDevice->isListening());
    if (!m_serverDevice->listen()) {
        return false;
    }

    return true;
}

bool Server::isListening() const
{
    return m_serverDevice->isListening();
}

Server *Server::instance()
{
    Q_ASSERT(s_instance);
    return static_cast<Server *>(s_instance);
}

bool Server::isRemoteClient() const
{
    return false;
}

QUrl Server::serverAddress() const
{
#ifdef Q_OS_ANDROID
    const QString defaultServerAddr = QLatin1String("local://") + QDir::homePath() + QLatin1String("/+gammaray_socket");
#else
    const QString defaultServerAddr = QString::fromUtf8(GAMMARAY_DEFAULT_ANY_TCP_URL);
#endif
    QUrl url(ProbeSettings::value(QStringLiteral("ServerAddress"), defaultServerAddr).toString());
    if (url.scheme().isEmpty())
        url.setScheme(QStringLiteral("tcp"));
    if (url.port() <= 0)
        url.setPort(defaultPort());
    return url;
}

void Server::newConnection()
{
    if (isConnected()) {
        cerr << Q_FUNC_INFO << " connected already, refusing incoming connection." << endl;
        auto con = m_serverDevice->nextPendingConnection();
        con->close();
        con->deleteLater();
        return;
    }

    m_broadcastTimer->stop();
    auto con = m_serverDevice->nextPendingConnection();
    // FIXME Use proper type for m_serverDevice->nextPendingConnection, instead
    // of relying on runtime-connect to a slot which doesn't exist in QIODevice
    connect(con, SIGNAL(disconnected()), con, SLOT(deleteLater()));
    setDevice(con);

    sendServerGreeting();

    emit connectionEstablished();
}

void Server::sendServerGreeting()
{
    // send greeting message for protocol version check
    {
        Message msg(endpointAddress(), Protocol::ServerVersion);
        msg << Protocol::version();
        send(msg);
    }

    {
        Message msg(endpointAddress(), Protocol::ServerInfo);
        msg << label() << key() << pid() << Message::highestSupportedDataVersion(); // TODO: expand with anything else needed here: Qt/GammaRay version, hostname, that kind of stuff
        send(msg);
    }

    {
        Message msg(endpointAddress(), Protocol::ObjectMapReply);
        msg << objectAddresses();
        send(msg);
    }
}

void Server::messageReceived(const Message &msg)
{
    if (msg.address() == endpointAddress()) {
        switch (msg.type()) {
        case Protocol::ClientDataVersionNegotiated:
        {
            quint8 version;
            msg >> version;

            {
                Message msg(endpointAddress(), Protocol::ServerDataVersionNegotiated);
                msg << version;
                send(msg);
            }

            Message::setNegotiatedDataVersion(version);
            break;
        }
        case Protocol::ObjectMonitored:
        case Protocol::ObjectUnmonitored:
        {
            Protocol::ObjectAddress addr;
            msg >> addr;
            Q_ASSERT(addr > Protocol::InvalidObjectAddress);
            m_propertySyncer->setObjectEnabled(addr, msg.type() == Protocol::ObjectMonitored);
            auto it = m_monitorNotifiers.constFind(addr);
            if (it == m_monitorNotifiers.constEnd())
                break;
            // cout << Q_FUNC_INFO << " un/monitor " << (int)addr << endl;
            QMetaObject::invokeMethod(it.value().first, it.value().second,
                                      Q_ARG(bool, msg.type() == Protocol::ObjectMonitored));
            break;
        }
        }
    } else {
        dispatchMessage(msg);
    }
}

void Server::invokeObject(const QString &objectName, const char *method,
                          const QVariantList &args) const
{
    Endpoint::invokeObject(objectName, method, args);

    QObject *object = ObjectBroker::objectInternal(objectName);
    Q_ASSERT(object);
    // also invoke locally for in-process mode
    invokeObjectLocal(object, method, args);
}

Protocol::ObjectAddress Server::registerObject(const QString &name, QObject *object)
{
    return registerObject(name, object, ExportEverything);
}

static bool isNotifySignal(const QMetaObject *mo, const QMetaMethod &method)
{
    for (int i = 0; i < mo->propertyCount(); ++i) {
        const auto prop = mo->property(i);
        if (!prop.hasNotifySignal())
            continue;
        if (prop.notifySignal().methodIndex() == method.methodIndex())
            return true;
    }
    return false;
}

Protocol::ObjectAddress Server::registerObject(const QString &name, QObject *object,
                                               Server::ObjectExportOptions exportOptions)
{
    addObjectNameAddressMapping(name, ++m_nextAddress);
    Protocol::ObjectAddress address = Endpoint::registerObject(name, object);
    Q_ASSERT(m_nextAddress);
    Q_ASSERT(m_nextAddress == address);

    if (isConnected()) {
        Message msg(endpointAddress(), Protocol::ObjectAdded);
        msg <<  name << m_nextAddress;
        send(msg);
    }

    if (exportOptions & ExportSignals) {
        const QMetaObject *meta = object->metaObject();
        for (int i = 0; i < meta->methodCount(); ++i) {
            const QMetaMethod method = meta->method(i);
            if (method.methodType() != QMetaMethod::Signal)
                continue;
            if ((exportOptions & ExportProperties) && isNotifySignal(meta, method))
                continue; // no need to forward property change signals if we forward the property already
            m_signalMapper->connectToSignal(object, method);
        }
    }

    if (exportOptions & ExportProperties)
        m_propertySyncer->addObject(address, object);

    return address;
}

void Server::forwardSignal(QObject *sender, int signalIndex, const QVector< QVariant > &args)
{
    if (!isConnected())
        return;

    Q_ASSERT(sender);
    Q_ASSERT(signalIndex >= 0);
    const QMetaMethod signal = sender->metaObject()->method(signalIndex);
    Q_ASSERT(signal.methodType() == QMetaMethod::Signal);

    QByteArray name = signal.methodSignature();
    // get the name of the function to invoke, excluding the parens and function arguments.
    name = name.mid(0, name.indexOf('('));

    QVariantList v;
    v.reserve(args.size());
    foreach (const QVariant &arg, args)
        v.push_back(arg);
    Endpoint::invokeObject(sender->objectName(), name, v);
}

void Server::registerMonitorNotifier(Protocol::ObjectAddress address, QObject *receiver,
                                     const char *monitorNotifier)
{
    Q_ASSERT(address != Protocol::InvalidObjectAddress);
    Q_ASSERT(receiver);
    Q_ASSERT(monitorNotifier);

    m_monitorNotifiers.insert(address, qMakePair<QObject *, QByteArray>(receiver, monitorNotifier));
}

void Server::handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString &objectName)
{
    removeObjectNameAddressMapping(objectName);
    m_monitorNotifiers.remove(objectAddress);

    if (isConnected()) {
        Message msg(endpointAddress(), Protocol::ObjectRemoved);
        msg << objectName;
        send(msg);
    }
}

void Server::objectDestroyed(Protocol::ObjectAddress /*objectAddress*/, const QString &objectName,
                             QObject *object)
{
    Q_UNUSED(object);
    removeObjectNameAddressMapping(objectName);

    if (isConnected()) {
        Message msg(endpointAddress(), Protocol::ObjectRemoved);
        msg << objectName;
        send(msg);
    }
}

void Server::broadcast()
{
    if (!Server::instance()->isListening())
        return;

    QByteArray datagram;
    QDataStream stream(&datagram, QIODevice::WriteOnly);
    stream << Protocol::broadcastFormatVersion();
    stream << Protocol::version();
    stream << externalAddress();
    stream << label(); // TODO integrate hostname
    m_serverDevice->broadcast(datagram);
}

QUrl Server::externalAddress() const
{
    if (!m_serverDevice)
        return QUrl();
    return m_serverDevice->externalAddress();
}

QString Server::errorString() const
{
    if (!m_serverDevice)
        return QString();
    return m_serverDevice->errorString();
}
