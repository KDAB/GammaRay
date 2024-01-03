/*
  quickinspectorclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "quickinspectorclient.h"
#include "quickdecorationsdrawer.h"

#include <common/endpoint.h>

#include <QEvent>

using namespace GammaRay;

QuickInspectorClient::QuickInspectorClient(QObject *parent)
    : QuickInspectorInterface(parent)
{
}

QuickInspectorClient::~QuickInspectorClient() = default;

void QuickInspectorClient::selectWindow(int index)
{
    Endpoint::instance()->invokeObject(objectName(), "selectWindow", QVariantList() << index);
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

void QuickInspectorClient::setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings)
{
    Endpoint::instance()->invokeObject(objectName(),
                                       "setOverlaySettings",
                                       QVariantList()
                                           << QVariant::fromValue(settings));
}

void QuickInspectorClient::checkOverlaySettings()
{
    Endpoint::instance()->invokeObject(objectName(), "checkOverlaySettings");
}

void QuickInspectorClient::analyzePainting()
{
    Endpoint::instance()->invokeObject(objectName(), "analyzePainting");
}

void QuickInspectorClient::checkSlowMode()
{
    Endpoint::instance()->invokeObject(objectName(), "checkSlowMode");
}

void QuickInspectorClient::setSlowMode(bool slow)
{
    Endpoint::instance()->invokeObject(objectName(),
                                       "setSlowMode",
                                       QVariantList()
                                           << slow);
}
