/*
  probeguard.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "probeguard.h"

#include <QThreadStorage>

static QThreadStorage<bool> s_probeGuards;

using namespace GammaRay;

ProbeGuard::ProbeGuard()
    : m_previousState(insideProbe())
{
    setInsideProbe(true);
}

ProbeGuard::ProbeGuard(bool newState)
    : m_previousState(insideProbe())
{
    setInsideProbe(newState);
}

ProbeGuard::~ProbeGuard()
{
    setInsideProbe(m_previousState);
}

bool ProbeGuard::insideProbe()
{
    if (!s_probeGuards.hasLocalData())
        return false;
    return s_probeGuards.localData();
}

void ProbeGuard::setInsideProbe(bool inside)
{
    s_probeGuards.localData() = inside;
}

ProbeGuardSuspender::ProbeGuardSuspender()
    : ProbeGuard(false)
{
}
ProbeGuardSuspender::~ProbeGuardSuspender() = default;
