/*
  quickinspectorclient.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "quickinspectorclient.h"
#include <common/endpoint.h>

#include <QEvent>

using namespace GammaRay;

QuickInspectorClient::QuickInspectorClient(QObject *parent) : QuickInspectorInterface(parent)
{
}

QuickInspectorClient::~QuickInspectorClient()
{
}

void QuickInspectorClient::selectWindow(int index)
{
  Endpoint::instance()->invokeObject(objectName(), "selectWindow", QVariantList() << index);
}

void QuickInspectorClient::renderScene()
{
  Endpoint::instance()->invokeObject(objectName(), "renderScene");
}

void QuickInspectorClient::sendKeyEvent(int type, int key, int modifiers, const QString &text,
                                        bool autorep, ushort count)
{
  Endpoint::instance()->invokeObject(objectName(),
                                     "sendKeyEvent",
                                     QVariantList()
                                       << QVariant::fromValue(type)
                                       << QVariant::fromValue(key)
                                       << QVariant::fromValue(modifiers)
                                       << QVariant::fromValue(text)
                                       << QVariant::fromValue(autorep)
                                       << QVariant::fromValue(count));
}

void QuickInspectorClient::sendMouseEvent(int type, const QPointF &localPos, int button,
                                          int buttons, int modifiers)
{
  Endpoint::instance()->invokeObject(objectName(),
                                     "sendMouseEvent",
                                      QVariantList()
                                        << QVariant::fromValue(type)
                                        << QVariant::fromValue(localPos)
                                        << QVariant::fromValue(button)
                                        << QVariant::fromValue(buttons)
                                        << QVariant::fromValue(modifiers));
}

void QuickInspectorClient::sendWheelEvent(const QPointF &localPos, QPoint pixelDelta,
                                          QPoint angleDelta, int buttons, int modifiers)
{
  Endpoint::instance()->invokeObject(objectName(),
                                     "sendWheelEvent",
                                     QVariantList()
                                       << QVariant::fromValue(localPos)
                                       << QVariant::fromValue(pixelDelta)
                                       << QVariant::fromValue(angleDelta)
                                       << QVariant::fromValue(buttons)
                                       << QVariant::fromValue(modifiers));
}

void QuickInspectorClient::setCustomRenderMode(
  GammaRay::QuickInspectorInterface::RenderMode customRenderMode)
{
  Endpoint::instance()->invokeObject(objectName(),
                                     "setCustomRenderMode",
                                     QVariantList()
                                       << QVariant::fromValue(customRenderMode));
}

void QuickInspectorClient::checkFeatures()
{
  Endpoint::instance()->invokeObject(objectName(), "checkFeatures");
}

void QuickInspectorClient::setSceneViewActive(bool active)
{
  Endpoint::instance()->invokeObject(objectName(), "setSceneViewActive", QVariantList() << QVariant::fromValue(active));
}
