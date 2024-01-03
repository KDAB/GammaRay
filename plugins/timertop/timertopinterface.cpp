/*
  timertopinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "timertopinterface.h"

#include <common/objectbroker.h>

namespace GammaRay {
TimerTopInterface::TimerTopInterface(QObject *parent)
    : QObject(parent)
{
    ObjectBroker::registerObject<TimerTopInterface *>(this);
}

TimerTopInterface::~TimerTopInterface() = default;
}
