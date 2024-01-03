/*
  wlcompositorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "wlcompositorinterface.h"

#include <common/objectbroker.h>

namespace GammaRay {

WlCompositorInterface::WlCompositorInterface(QObject *parent)
    : QObject(parent)
{
    ObjectBroker::registerObject<WlCompositorInterface *>(this);
}

WlCompositorInterface::~WlCompositorInterface() = default;

}
