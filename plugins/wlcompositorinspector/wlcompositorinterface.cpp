/*
  wlcompositorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
