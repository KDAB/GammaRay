/*
  remoteviewserver.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "remoteviewserver.h"

#include <common/remoteviewframe.h>

#include <core/remote/server.h>

#include <QCoreApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QWindow>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <private/qeventpoint_p.h>
#include <private/qevent_p.h>
#include <private/qpointingdevice_p.h>
#endif

using namespace GammaRay;

RemoteViewServer::RemoteViewServer(const QString &name, QObject *parent)
    : RemoteViewInterface(name, parent)
    , m_eventReceiver(nullptr)
    , m_updateTimer(new QTimer(this))
    , m_clientActive(false)
    , m_sourceChanged(false)
    , m_clientReady(true)
    , m_grabberReady(true)
    , m_pendingReset(false)
    , m_pendingCompleteFrame(false)
{
    Server::instance()->registerMonitorNotifier(Endpoint::instance()->objectAddress(
                                                    name),
                                                this, "clientConnectedChanged");

    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(10);
    connect(m_updateTimer, &QTimer::timeout, this, &RemoteViewServer::requestUpdateTimeout);
}

void RemoteViewServer::setEventReceiver(EventReceiver *receiver)
{
    m_eventReceiver = receiver;
}

void RemoteViewServer::requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode)
{
    emit elementsAtRequested(pos, mode);
}

void RemoteViewServer::pickElementId(const GammaRay::ObjectId &id)
{
    emit doPickElementId(id);
}

void RemoteViewServer::resetView()
{
    if (isActive())
        emit reset();
    else
        m_pendingReset = true;
}

bool RemoteViewServer::isActive() const
{
    return m_clientActive;
}

void RemoteViewServer::setGrabberReady(bool ready)
{
    if (ready == m_grabberReady)
        return;
    m_grabberReady = ready;
    checkRequestUpdate();
}

void RemoteViewServer::sendFrame(const RemoteViewFrame &frame)
{
    m_clientReady = false;

    const QSize frameImageSize = frame.image().size() / frame.image().devicePixelRatio();
    m_lastTransmittedViewRect = frame.viewRect();
    m_lastTransmittedImageRect = frame.transform().mapRect(QRect(QPoint(), frameImageSize));

    if (m_pendingCompleteFrame && frameImageSize == frame.viewRect().size())
        m_pendingCompleteFrame = false;
    emit frameUpdated(frame);
}

QRectF RemoteViewServer::userViewport() const
{
    return m_pendingCompleteFrame ? QRectF() : m_userViewport;
}

void RemoteViewServer::sourceChanged()
{
    m_sourceChanged = true;
    checkRequestUpdate();
}

void RemoteViewServer::requestCompleteFrame()
{
    if (m_pendingCompleteFrame)
        return;
    m_pendingCompleteFrame = true;
    sourceChanged();
}

void RemoteViewServer::clientViewUpdated()
{
    m_clientReady = true;
    m_sourceChanged = m_sourceChanged || m_pendingCompleteFrame;
    checkRequestUpdate();
}

void RemoteViewServer::checkRequestUpdate()
{
    if (isActive() && !m_updateTimer->isActive() && m_clientReady && m_grabberReady && m_sourceChanged)
        m_updateTimer->start();
}

void RemoteViewServer::sendKeyEvent(int type, int key, int modifiers, const QString &text,
                                    bool autorep, ushort count)
{
    if (!m_eventReceiver)
        return;

    auto event = new QKeyEvent(( QEvent::Type )type, key, ( Qt::KeyboardModifiers )modifiers, text,
                               autorep, count);
    QCoreApplication::postEvent(m_eventReceiver, event);
}

void RemoteViewServer::sendMouseEvent(int type, const QPoint &localPos, int button, int buttons,
                                      int modifiers)
{
    if (!m_eventReceiver)
        return;

    auto event = new QMouseEvent(( QEvent::Type )type, localPos, ( Qt::MouseButton )button,
                                 ( Qt::MouseButtons )buttons, ( Qt::KeyboardModifiers )modifiers);
    QCoreApplication::postEvent(m_eventReceiver, event);
}

void RemoteViewServer::sendWheelEvent(const QPoint &localPos, QPoint pixelDelta, QPoint angleDelta,
                                      int buttons, int modifiers)
{
    if (!m_eventReceiver)
        return;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    auto event = new QWheelEvent(localPos, m_eventReceiver->mapToGlobal(localPos), pixelDelta, angleDelta, ( Qt::MouseButtons )buttons,
                                 ( Qt::KeyboardModifiers )modifiers, Qt::NoScrollPhase, false);
#else
    auto event = new QWheelEvent(localPos, m_eventReceiver->mapToGlobal(localPos), pixelDelta, angleDelta, 0, /*not used*/ Qt::Vertical,
                                 /*not used*/ ( Qt::MouseButtons )buttons,
                                 ( Qt::KeyboardModifiers )modifiers);
#endif
    QCoreApplication::postEvent(m_eventReceiver, event);
}

void RemoteViewServer::sendTouchEvent(int type, int touchDeviceType, int deviceCaps, int touchDeviceMaxTouchPoints,
                                      int modifiers, int touchPointStates, const QList<QTouchEvent::TouchPoint> &touchPoints)
{
    if (!m_eventReceiver)
        return;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    if (!m_touchDevice) {
        // create our own touch device, the system may not have one already, or it may not have
        // the properties we want
        m_touchDevice.reset(new QPointingDevice);
    }
    m_touchDevice->setType(QPointingDevice::DeviceType(touchDeviceType));
    m_touchDevice->setCapabilities(QPointingDevice::Capabilities(deviceCaps));
    m_touchDevice->setMaximumTouchPoints(touchDeviceMaxTouchPoints);

    const QEventPoint::States states(touchPointStates);
    auto event = new QTouchEvent(QEvent::Type(type), m_touchDevice.get(), Qt::KeyboardModifiers(modifiers), states, touchPoints);

    auto *mut = QMutableTouchEvent::from(event);
    mut->setTarget(m_eventReceiver);
#else
    if (!m_touchDevice) {
        // create our own touch device, the system may not have one already, or it may not have
        // the properties we want
        m_touchDevice.reset(new QTouchDevice);
    }
    m_touchDevice->setType(QTouchDevice::DeviceType(touchDeviceType));
    m_touchDevice->setCapabilities(QTouchDevice::CapabilityFlag(deviceCaps));
    m_touchDevice->setMaximumTouchPoints(touchDeviceMaxTouchPoints);

    const Qt::TouchPointStates states(touchPointStates);
    auto event = new QTouchEvent(QEvent::Type(type), m_touchDevice.get(), Qt::KeyboardModifiers(modifiers), states, touchPoints);
    event->setWindow(m_eventReceiver);
#endif

    QCoreApplication::sendEvent(m_eventReceiver, event);
}

void RemoteViewServer::setViewActive(bool active)
{
    if (m_pendingReset) {
        emit reset();
        m_pendingReset = false;
    }

    m_clientActive = active;
    m_clientReady = active;
    m_pendingCompleteFrame = false;
    if (active)
        sourceChanged();
    else
        m_updateTimer->stop();
}

void RemoteViewServer::sendUserViewport(const QRectF &userViewport)
{
    m_userViewport = userViewport;
    auto newlyRequestedRect = userViewport.intersected(m_lastTransmittedViewRect);
    if (!m_lastTransmittedImageRect.contains(newlyRequestedRect))
        sourceChanged();
}

void RemoteViewServer::clientConnectedChanged(bool connected)
{
    if (!connected)
        setViewActive(false);
}

void RemoteViewServer::requestUpdateTimeout()
{
    m_sourceChanged = false;
    emit requestUpdate();
}
