/*
  probecontrollerclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "probecontrollerclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

ProbeControllerClient::ProbeControllerClient(QObject *parent)
    : ProbeControllerInterface(parent)
{
}

void ProbeControllerClient::detachProbe()
{
    Endpoint::instance()->invokeObject(objectName(), "detachProbe");
}

void ProbeControllerClient::quitHost()
{
    Endpoint::instance()->invokeObject(objectName(), "quitHost");
}
