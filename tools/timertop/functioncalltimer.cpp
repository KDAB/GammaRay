/*
  functioncalltimer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

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
#include "functioncalltimer.h"

#include <qglobal.h>

using namespace GammaRay;

FunctionCallTimer::FunctionCallTimer()
  : m_active(false)
{
}

bool FunctionCallTimer::start()
{
  if (m_active) {
    return false;
  }

  clock_gettime(CLOCK_REALTIME, &m_startTime);
  m_active = true;
  return true;
}

bool FunctionCallTimer::active() const
{
  return m_active;
}

int FunctionCallTimer::stop()
{
  Q_ASSERT(m_active);
  m_active = false;
  timespec endTime;
  clock_gettime(CLOCK_REALTIME, &endTime);
  int elapsed = (endTime.tv_nsec - m_startTime.tv_nsec) / 1000;
  elapsed += (endTime.tv_sec - m_startTime.tv_sec) * 1000000;
  return elapsed;
}
