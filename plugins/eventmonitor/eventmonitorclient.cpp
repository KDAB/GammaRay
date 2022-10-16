/*
  eventmonitorclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "eventmonitorclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

GammaRay::EventMonitorClient::EventMonitorClient(QObject *parent)
    : EventMonitorInterface(parent)
{
}

GammaRay::EventMonitorClient::~EventMonitorClient() = default;


void GammaRay::EventMonitorClient::clearHistory()
{
    Endpoint::instance()->invokeObject(objectName(), "clearHistory");
}

void EventMonitorClient::recordAll()
{
    Endpoint::instance()->invokeObject(objectName(), "recordAll");
}

void EventMonitorClient::recordNone()
{
    Endpoint::instance()->invokeObject(objectName(), "recordNone");
}

void EventMonitorClient::showAll()
{
    Endpoint::instance()->invokeObject(objectName(), "showAll");
}

void EventMonitorClient::showNone()
{
    Endpoint::instance()->invokeObject(objectName(), "showNone");
}
