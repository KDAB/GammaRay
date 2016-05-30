/*
  remoteviewserver.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "remoteviewserver.h"

#include <core/remote/server.h>

#include <QCoreApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QWindow>
#endif

using namespace GammaRay;

RemoteViewServer::RemoteViewServer(const QString &name, QObject *parent)
    : RemoteViewInterface(name, parent)
    , m_eventReceiver(Q_NULLPTR)
    , m_updateTimer(new QTimer(this))
    , m_clientActive(false)
    , m_sourceChanged(false)
    , m_clientReady(true)
{
    Server::instance()->registerMonitorNotifier(Endpoint::instance()->objectAddress(
                                                    name), this, "clientConnectedChanged");

    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(100);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(requestUpdateTimeout()));
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
}

bool RemoteViewServer::isActive() const
{
    return m_clientActive;
}

void RemoteViewServer::sendFrame(const RemoteViewFrame &frame)
{
    m_clientReady = false;
    emit frameUpdated(frame);
}

void RemoteViewServer::sourceChanged()
{
    m_sourceChanged = true;
    checkRequestUpdate();
}

void RemoteViewServer::clientViewUpdated()
{
    m_clientReady = true;
    checkRequestUpdate();
}

void RemoteViewServer::checkRequestUpdate()
{
    if (isActive() && !m_updateTimer->isActive() && m_clientReady && m_sourceChanged)
        m_updateTimer->start();
}

void RemoteViewServer::sendKeyEvent(int type, int key, int modifiers, const QString &text,
                                    bool autorep, ushort count)
{
    if (!m_eventReceiver)
        return;

    auto event = new QKeyEvent((QEvent::Type)type, key, (Qt::KeyboardModifiers)modifiers, text,
                               autorep, count);
    QCoreApplication::postEvent(m_eventReceiver, event);
}

void RemoteViewServer::sendMouseEvent(int type, const QPoint &localPos, int button, int buttons,
                                      int modifiers)
{
    if (!m_eventReceiver)
        return;

    auto event
        = new QMouseEvent((QEvent::Type)type, localPos, (Qt::MouseButton)button,
                          (Qt::MouseButtons)buttons, (Qt::KeyboardModifiers)modifiers);
    QCoreApplication::postEvent(m_eventReceiver, event);
}

void RemoteViewServer::sendWheelEvent(const QPoint &localPos, QPoint pixelDelta, QPoint angleDelta,
                                      int buttons, int modifiers)
{
    if (!m_eventReceiver)
        return;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    auto event = new QWheelEvent(localPos, m_eventReceiver->mapToGlobal(
                                     localPos), pixelDelta, angleDelta, 0, /*not used*/ Qt::Vertical,
                                 /*not used*/ (Qt::MouseButtons)buttons,
                                 (Qt::KeyboardModifiers)modifiers);
#else
    Q_UNUSED(pixelDelta);
    auto orientation = angleDelta.x() == 0 ? Qt::Vertical : Qt::Horizontal;
    auto delta = orientation == Qt::Horizontal ? angleDelta.x() : angleDelta.y();
    auto event = new QWheelEvent(localPos, delta, (Qt::MouseButtons)buttons,
                                 (Qt::KeyboardModifiers)modifiers, orientation);
#endif
    QCoreApplication::sendEvent(m_eventReceiver, event);
}

void RemoteViewServer::setViewActive(bool active)
{
    m_clientActive = active;
    m_clientReady = active;
    if (active)
        sourceChanged();
    else
        m_updateTimer->stop();
}

void RemoteViewServer::clientConnectedChanged(bool connected)
{
    if (!connected)
        setViewActive(false);
}

void RemoteViewServer::requestUpdateTimeout()
{
    emit requestUpdate();
    m_sourceChanged = false;
}
