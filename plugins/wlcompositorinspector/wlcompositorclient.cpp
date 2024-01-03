/*
  wlcompositorclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "wlcompositorclient.h"

#include <common/endpoint.h>

namespace GammaRay {

WlCompositorClient::WlCompositorClient(QObject *p)
    : WlCompositorInterface(p)
{
}

void WlCompositorClient::connected()
{
    Endpoint::instance()->invokeObject(objectName(), "connected");
}

void WlCompositorClient::disconnected()
{
    Endpoint::instance()->invokeObject(objectName(), "disconnected");
}

void WlCompositorClient::setSelectedClient(int index)
{
    Endpoint::instance()->invokeObject(objectName(), "setSelectedClient", QVariantList() << index);
}

void WlCompositorClient::setSelectedResource(uint32_t id)
{
    Endpoint::instance()->invokeObject(objectName(), "setSelectedResource", QVariantList() << id);
}

}
