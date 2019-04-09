/*
  endpoint.cpp

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

#include "endpoint.h"
#include "message.h"
#include "methodargument.h"
#include "propertysyncer.h"

#include <iostream>

#include <QLoggingCategory>
//we use qCWarning, which we turn off by default, but which is not compiled out in releasebuilds
Q_LOGGING_CATEGORY(networkstatistics, "gammaray.network.statistics", QtMsgType::QtCriticalMsg)

using namespace GammaRay;
using namespace std;

Endpoint *Endpoint::s_instance = nullptr;

Endpoint::Endpoint(QObject *parent)
    : QObject(parent)
    , m_propertySyncer(new PropertySyncer(this))
    , m_socket(nullptr)
    , m_myAddress(Protocol::InvalidObjectAddress +1)
    , m_bytesRead(0)
    , m_bytesWritten(0)
    , m_pid(-1)
{
    if (s_instance) {
        qCritical(
            "Found existing GammaRay::Endpoint instance - trying to attach to a GammaRay client?");
    }

    Q_ASSERT(!s_instance);
    s_instance = this;

    auto *endpointObj = new ObjectInfo;
    endpointObj->address = m_myAddress;
    endpointObj->name = QStringLiteral("com.kdab.GammaRay.Server");
    // TODO: we could set this as message handler here and use the same dispatch mechanism
    insertObjectInfo(endpointObj);

    m_bandwidthMeasurementTimer = new QTimer(this);
    connect(m_bandwidthMeasurementTimer, &QTimer::timeout, this, &Endpoint::doLogTransmissionRate);
    m_bandwidthMeasurementTimer->start(1000);

    connect(m_propertySyncer, &PropertySyncer::message, this,
            &Endpoint::sendMessage);
}

Endpoint::~Endpoint()
{
    for (auto it = m_addressMap.constBegin(); it != m_addressMap.constEnd(); ++it) {
        delete it.value();
    }

    s_instance = nullptr;
}

Endpoint *Endpoint::instance()
{
    return s_instance;
}

void Endpoint::send(const Message &msg)
{
    Q_ASSERT(s_instance);
    s_instance->doSendMessage(msg);
}

void Endpoint::sendMessage(const Message &msg)
{
    if (!isConnected())
        return;

    doSendMessage(msg);
}

void Endpoint::doSendMessage(const GammaRay::Message &msg)
{
    Q_ASSERT(msg.address() != Protocol::InvalidObjectAddress);
    msg.write(m_socket);
    m_bytesWritten += msg.size();
}

void Endpoint::waitForMessagesWritten()
{
    m_socket->waitForBytesWritten(-1);
}

bool Endpoint::isConnected()
{
    return s_instance && s_instance->m_socket;
}

quint16 Endpoint::defaultPort()
{
    return 11732;
}

quint16 Endpoint::broadcastPort()
{
    return 13325;
}

void Endpoint::doLogTransmissionRate()
{
    emit logTransmissionRate(m_bytesRead, m_bytesWritten);

    if(!isRemoteClient()) {
        if(m_bytesRead != 0 || m_bytesWritten != 0) {
            const float transmissionRateRX = (m_bytesRead * 8 / 1024.0 / 1024.0); // in Mpbs
            const float transmissionRateTX = (m_bytesWritten * 8 / 1024.0 / 1024.0); // in Mpbs
            qCWarning(networkstatistics, "RX %7.3f Mbps | TX %7.3f Mbps", transmissionRateRX, transmissionRateTX);
        }
    }
    m_bytesRead = 0;
    m_bytesWritten = 0;
}

void Endpoint::setDevice(QIODevice *device)
{
    Q_ASSERT(!m_socket);
    Q_ASSERT(device);
    m_socket = device;
    connect(m_socket.data(), &QIODevice::readyRead, this, &Endpoint::readyRead);
    // FIXME Use proper type for m_socket, instead of relying on runtime-connect
    // to a slot which doesn't exist in QIODevice
    connect(m_socket.data(), SIGNAL(disconnected()), SLOT(connectionClosed()));
    if (m_socket->bytesAvailable())
        readyRead();
}

Protocol::ObjectAddress Endpoint::endpointAddress() const
{
    return m_myAddress;
}

void Endpoint::readyRead()
{
    while (Message::canReadMessage(m_socket.data())) {
        const auto msg = Message::readMessage(m_socket.data());
        m_bytesRead += msg.size();
        messageReceived(msg);
    }
}

void Endpoint::connectionClosed()
{
    disconnect(m_socket.data(), &QIODevice::readyRead, this, &Endpoint::readyRead);
    disconnect(m_socket.data(), SIGNAL(disconnected()), this, SLOT(connectionClosed()));
    m_socket = nullptr;
    emit disconnected();
}

Protocol::ObjectAddress Endpoint::objectAddress(const QString &objectName) const
{
    auto it = m_nameMap.constFind(objectName);
    if (it != m_nameMap.constEnd())
        return it.value()->address;

    return Protocol::InvalidObjectAddress;
}

Protocol::ObjectAddress Endpoint::registerObject(const QString &name, QObject *object)
{
    ObjectInfo *obj = m_nameMap.value(name, nullptr);
#if !defined(NDEBUG)
    Q_ASSERT(obj);
    Q_ASSERT(!obj->object);
    Q_ASSERT(obj->address != Protocol::InvalidObjectAddress);
#else
    if (!obj || obj->object || obj->address == Protocol::InvalidObjectAddress)
        return 0;
#endif

    obj->object = object;

    Q_ASSERT(!m_objectMap.contains(object));
    m_objectMap[object] = obj;

    connect(object, &QObject::destroyed, this, &Endpoint::slotObjectDestroyed);

    return obj->address;
}

void Endpoint::invokeObject(const QString &objectName, const char *method,
                            const QVariantList &args) const
{
    if (!isConnected())
        return;

    ObjectInfo *obj = m_nameMap.value(objectName, nullptr);
#if !defined(NDEBUG)
    Q_ASSERT(obj);
    Q_ASSERT(obj->address != Protocol::InvalidObjectAddress);
#else
    if (!obj || obj->address == Protocol::InvalidObjectAddress)
        return;
#endif

    Message msg(obj->address, Protocol::MethodCall);
    const QByteArray name(method);
    Q_ASSERT(!name.isEmpty());
    msg << name << args;
    send(msg);
}

void Endpoint::invokeObjectLocal(QObject *object, const char *method,
                                 const QVariantList &args) const
{
    Q_ASSERT(args.size() <= 10);
    QVector<MethodArgument> a(10);
    for (int i = 0; i < args.size(); ++i) {
        a[i] = MethodArgument(args.at(i));
    }

    QMetaObject::invokeMethod(object, method, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8],
                              a[9]);
}

void Endpoint::addObjectNameAddressMapping(const QString &objectName,
                                           Protocol::ObjectAddress objectAddress)
{
    Q_ASSERT(objectAddress != Protocol::InvalidObjectAddress);

    auto *obj = new ObjectInfo;
    obj->address = objectAddress;
    obj->name = objectName;
    insertObjectInfo(obj);

    emit objectRegistered(objectName, objectAddress);
}

void Endpoint::removeObjectNameAddressMapping(const QString &objectName)
{
    Q_ASSERT(m_nameMap.contains(objectName));
    ObjectInfo *obj = m_nameMap.value(objectName);

    emit objectUnregistered(objectName, obj->address);
    removeObjectInfo(obj);
}

void Endpoint::registerMessageHandler(Protocol::ObjectAddress objectAddress, QObject *receiver,
                                      const char *messageHandlerName)
{
    Q_ASSERT(m_addressMap.contains(objectAddress));
    ObjectInfo *obj = m_addressMap.value(objectAddress);
    Q_ASSERT(obj);
    Q_ASSERT(!obj->receiver);
    Q_ASSERT(!obj->messageHandler.isValid());
    obj->receiver = receiver;

    QByteArray signature(messageHandlerName);
    signature += "(GammaRay::Message)";
    auto idx = receiver->metaObject()->indexOfMethod(signature);
    Q_ASSERT(idx >= 0);
    obj->messageHandler = receiver->metaObject()->method(idx);

    Q_ASSERT(!m_handlerMap.contains(receiver, obj));
    m_handlerMap.insert(receiver, obj);
    if (obj->receiver != obj->object)
        connect(receiver, &QObject::destroyed, this, &Endpoint::slotHandlerDestroyed);
}

void Endpoint::unregisterMessageHandler(Protocol::ObjectAddress objectAddress)
{
    Q_ASSERT(m_addressMap.contains(objectAddress));
    ObjectInfo *obj = m_addressMap.value(objectAddress);
    Q_ASSERT(obj);
    Q_ASSERT(obj->receiver);
    disconnect(obj->receiver, &QObject::destroyed, this,
               &Endpoint::slotHandlerDestroyed);
    m_handlerMap.remove(obj->receiver, obj);
    obj->receiver = nullptr;
    obj->messageHandler = QMetaMethod();
}

void Endpoint::slotObjectDestroyed(QObject *obj)
{
    ObjectInfo *info = m_objectMap.value(obj, nullptr);
#if !defined(NDEBUG)
    Q_ASSERT(info);
    Q_ASSERT(info->object == obj);
#else
    if (!info || info->object != obj)
        return;
#endif

    info->object = nullptr;
    m_objectMap.remove(obj);
    // copy the name, in case unregisterMessageHandlerInternal() is called inside
    objectDestroyed(info->address, QString(info->name), obj);
}

void Endpoint::slotHandlerDestroyed(QObject *obj)
{
    // copy, the virtual method below likely changes the maps.
    const QList<ObjectInfo *> objs = m_handlerMap.values(obj);
    m_handlerMap.remove(obj);
    for (ObjectInfo *obj : objs) {
        obj->receiver = nullptr;
        obj->messageHandler = QMetaMethod();
        // copy the name, in case unregisterMessageHandlerInternal() is called inside
        handlerDestroyed(obj->address, QString(obj->name));
    }
}

void Endpoint::dispatchMessage(const Message &msg)
{
    auto it = m_addressMap.constFind(msg.address());
    if (it == m_addressMap.constEnd()) {
        cerr << "message for unknown object address received: " << quint64(msg.address()) << endl;
        return;
    }

    ObjectInfo *obj = it.value();
    if (msg.type() == Protocol::MethodCall) {
        QByteArray method;
        msg >> method;
        if (obj->object) {
            Q_ASSERT(!method.isEmpty());
            QVariantList args;
            msg >> args;

            invokeObjectLocal(obj->object, method.constData(), args);
        } else {
            cerr << "cannot call method " << method.constData() << " on unknown object of name "
                 << qPrintable(obj->name) << " with address " << quint64(obj->address)
                 << " - did you forget to register it?" << endl;
        }
    }

    if (obj->receiver)
        obj->messageHandler.invoke(obj->receiver, Q_ARG(GammaRay::Message, msg));

    if (!obj->receiver && (msg.type() != Protocol::MethodCall || !obj->object)) {
        cerr << "Cannot dispatch message " << quint64(msg.type()) << " - no handler registered."
             << " Receiver: " << qPrintable(obj->name) << ", address " << quint64(obj->address)
             << endl;
    }
}

QVector< QPair< Protocol::ObjectAddress, QString > > Endpoint::objectAddresses() const
{
    QVector<QPair<Protocol::ObjectAddress, QString> > addrs;
    addrs.reserve(m_addressMap.size());
    for (auto it = m_addressMap.constBegin(); it != m_addressMap.constEnd(); ++it) {
        addrs.push_back(qMakePair(it.key(), it.value()->name));
    }
    return addrs;
}

void Endpoint::insertObjectInfo(Endpoint::ObjectInfo *oi)
{
    Q_ASSERT(!m_addressMap.contains(oi->address));
    m_addressMap.insert(oi->address, oi);
    Q_ASSERT(!m_nameMap.contains(oi->name));
    m_nameMap.insert(oi->name, oi);

    if (oi->receiver)
        m_handlerMap.insert(oi->receiver, oi);

    if (oi->object)
        m_objectMap.insert(oi->object, oi);
}

void Endpoint::removeObjectInfo(Endpoint::ObjectInfo *oi)
{
    Q_ASSERT(m_addressMap.contains(oi->address));
    m_addressMap.remove(oi->address);
    Q_ASSERT(m_nameMap.contains(oi->name));
    m_nameMap.remove(oi->name);

    if (oi->receiver) {
        disconnect(oi->receiver, &QObject::destroyed, this,
                   &Endpoint::slotHandlerDestroyed);
        m_handlerMap.remove(oi->receiver, oi);
    }

    if (oi->object) {
        disconnect(oi->object, &QObject::destroyed, this,
                   &Endpoint::slotObjectDestroyed);
        m_objectMap.remove(oi->object);
    }

    delete oi;
}

QString Endpoint::label() const
{
    return m_label;
}

void Endpoint::setLabel(const QString &label)
{
    m_label = label;
}

QString Endpoint::key() const
{
    return m_key;
}

void Endpoint::setKey(const QString &key)
{
    m_key = key;
}

qint64 Endpoint::pid() const
{
    return m_pid;
}

void Endpoint::setPid(qint64 pid)
{
    m_pid = pid;
}
