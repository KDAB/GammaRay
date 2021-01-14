/*
  remoteviewclient.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "remoteviewclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

RemoteViewClient::RemoteViewClient(const QString &name, QObject *parent)
    : RemoteViewInterface(name, parent)
{
}

void RemoteViewClient::requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode)
{
    Endpoint::instance()->invokeObject(name(), "requestElementsAt", QVariantList() << pos << QVariant::fromValue(mode));
}

void RemoteViewClient::pickElementId(const GammaRay::ObjectId &id)
{
    Endpoint::instance()->invokeObject(name(), "pickElementId", QVariantList() << QVariant::fromValue(id));
}

void RemoteViewClient::sendKeyEvent(int type, int key, int modifiers, const QString &text,
                                    bool autorep, ushort count)
{
    Endpoint::instance()->invokeObject(name(), "sendKeyEvent", QVariantList()
                                       << QVariant::fromValue(type)
                                       << QVariant::fromValue(key)
                                       << QVariant::fromValue(modifiers)
                                       << QVariant::fromValue(text)
                                       << QVariant::fromValue(autorep)
                                       << QVariant::fromValue(count));
}

void RemoteViewClient::sendMouseEvent(int type, const QPoint &localPos, int button, int buttons,
                                      int modifiers)
{
    Endpoint::instance()->invokeObject(name(), "sendMouseEvent", QVariantList()
                                       << QVariant::fromValue(type)
                                       << QVariant::fromValue(localPos)
                                       << QVariant::fromValue(button)
                                       << QVariant::fromValue(buttons)
                                       << QVariant::fromValue(modifiers));
}

void RemoteViewClient::sendWheelEvent(const QPoint &localPos, QPoint pixelDelta, QPoint angleDelta,
                                      int buttons, int modifiers)
{
    Endpoint::instance()->invokeObject(name(), "sendWheelEvent", QVariantList()
                                       << QVariant::fromValue(localPos)
                                       << QVariant::fromValue(pixelDelta)
                                       << QVariant::fromValue(angleDelta)
                                       << QVariant::fromValue(buttons)
                                       << QVariant::fromValue(modifiers));
}

void RemoteViewClient::sendTouchEvent(int type, int touchDeviceType, int deviceCaps, int touchDeviceMaxTouchPoints, int modifiers,
                                      Qt::TouchPointStates touchPointStates, const QList<QTouchEvent::TouchPoint> &touchPoints)
{
    Endpoint::instance()->invokeObject(name(), "sendTouchEvent", QVariantList()
                                       << QVariant::fromValue(type)
                                       << QVariant::fromValue(touchDeviceType)
                                       << QVariant::fromValue(deviceCaps)
                                       << QVariant::fromValue(touchDeviceMaxTouchPoints)
                                       << QVariant::fromValue(modifiers)
                                       << QVariant::fromValue(touchPointStates)
                                       << QVariant::fromValue(touchPoints));
}

void RemoteViewClient::setViewActive(bool active)
{
    Endpoint::instance()->invokeObject(name(), "setViewActive", QVariantList() << active);
}

void RemoteViewClient::sendUserViewport(const QRectF &userViewport)
{
    Endpoint::instance()->invokeObject(name(), "sendUserViewport", QVariantList() << userViewport);
}

void RemoteViewClient::clientViewUpdated()
{
    Endpoint::instance()->invokeObject(name(), "clientViewUpdated");
}

void RemoteViewClient::requestCompleteFrame()
{
    Endpoint::instance()->invokeObject(name(), "requestCompleteFrame");
}
