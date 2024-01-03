/*
  probecontrollerclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
