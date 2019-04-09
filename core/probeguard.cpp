/*
  probeguard.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    : ProbeGuard(false) {}
ProbeGuardSuspender::~ProbeGuardSuspender() = default;
