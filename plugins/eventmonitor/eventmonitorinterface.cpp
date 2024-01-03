/*
  eventmonitorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "eventmonitorinterface.h"
#include <common/objectbroker.h>

using namespace GammaRay;

EventMonitorInterface::EventMonitorInterface(QObject *parent)
    : QObject(parent)
    , m_isPaused(false)
{
    ObjectBroker::registerObject<EventMonitorInterface *>(this);
}

void EventMonitorInterface::setIsPaused(bool value)
{
    m_isPaused = value;
    emit isPausedChanged();
}

EventMonitorInterface::~EventMonitorInterface() = default;
