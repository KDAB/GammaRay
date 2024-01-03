/*
  probecontrollerinterface.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "probecontrollerinterface.h"

using namespace GammaRay;

ProbeControllerInterface::ProbeControllerInterface(QObject *parent)
    : QObject(parent)
{
}

ProbeControllerInterface::~ProbeControllerInterface() = default;
