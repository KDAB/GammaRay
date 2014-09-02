/*
  functioncalltimer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#if defined(Q_OS_MAC) && defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach.h>
#endif

using namespace GammaRay;

#ifndef Q_OS_WIN
static void portableGetTimeUnix(timespec &t)
{
#if defined(Q_OS_MAC)
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  t.tv_sec = mts.tv_sec;
  t.tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_REALTIME, &t);
#endif
}
#endif

FunctionCallTimer::FunctionCallTimer()
  : m_startTime()
  , m_active(false)
{
}

bool FunctionCallTimer::start()
{
  if (m_active) {
    return false;
  }

#if defined(Q_OS_WIN)
  LARGE_INTEGER startTime;
  bool ret = QueryPerformanceCounter(&startTime);
  if (!ret) {
    return false;
  }
  m_startTime = startTime.QuadPart;
#else
  portableGetTimeUnix(m_startTime);
#endif
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

#if defined(Q_OS_WIN)
  LARGE_INTEGER endTime;
  LARGE_INTEGER frequency;
  QueryPerformanceCounter(&endTime);
  QueryPerformanceFrequency(&frequency);
  int elapsed = ((endTime.QuadPart - m_startTime) * 1000000) / frequency.QuadPart;
#else
  timespec endTime;
  portableGetTimeUnix(endTime);
  int elapsed = (endTime.tv_nsec - m_startTime.tv_nsec) / 1000;
  elapsed += (endTime.tv_sec - m_startTime.tv_sec) * 1000000;
#endif
  return elapsed;
}
