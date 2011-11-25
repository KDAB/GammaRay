/*
  timerinfo.cpp

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
#include "timerinfo.h"

using namespace GammaRay;

static const int maxTimeoutEvents = 1000;
static const int maxTimeSpan = 10000;

TimerInfo::TimerInfo(QTimer *timer)
  : m_type(QTimerType),
    m_timer(timer),
    m_timerId(timer->timerId()),
    m_totalWakeups(0)
{
}

TimerInfo::TimerInfo(int timerId)
  : m_type(QObjectType),
    m_timerId(timerId),
    m_totalWakeups(0)
{
}

void TimerInfo::addEvent(const TimeoutEvent &timeoutEvent)
{
  m_timeoutEvents.append(timeoutEvent);
  removeOldEvents();
  m_totalWakeups++;
}

int TimerInfo::numEvents() const
{
  return m_timeoutEvents.size();
}

QTimer *TimerInfo::timer() const
{
  return m_timer;
}

int TimerInfo::timerId() const
{
  return m_timerId;
}

FunctionCallTimer *TimerInfo::functionCallTimer()
{
  return &m_functionCallTimer;
}

float TimerInfo::wakeupsPerSec() const
{
  int totalWakeups = 0;
  int start = 0;
  int end = m_timeoutEvents.size() - 1;
  for (int i = end; i >= 0; i--) {
    const TimeoutEvent &event = m_timeoutEvents.at(i);
    if (event.timeStamp.msecsTo(QTime::currentTime()) > maxTimeSpan) {
      start = i;
      break;
    }
    totalWakeups++;
  }

  if (totalWakeups > 0 && end > start) {
    const QTime startTime = m_timeoutEvents[start].timeStamp;
    const QTime endTime = m_timeoutEvents[end].timeStamp;
    const int timeSpan = startTime.msecsTo(endTime);
    const float wakeupsPerSec = totalWakeups / (float)timeSpan * 1000.0f;
    return wakeupsPerSec;
  }
  return 0;
}

int TimerInfo::timePerWakeup() const
{
  int totalWakeups = 0;
  int totalTime = 0;
  for (int i = m_timeoutEvents.size() - 1; i >= 0; i--) {
    const TimeoutEvent &event = m_timeoutEvents.at(i);
    if (event.timeStamp.msecsTo(QTime::currentTime()) > maxTimeSpan) {
      break;
    }
    totalWakeups++;
    totalTime += event.executionTime;
  }

  if (totalWakeups > 0) {
    return totalTime / (float)totalWakeups;
  }
  return 0;
}

int TimerInfo::maxWakeupTime() const
{
  int max = 0;
  for (int i = 0; i < m_timeoutEvents.size(); i++) {
    const TimeoutEvent &event = m_timeoutEvents.at(i);
    if (event.executionTime > max) {
      max = event.executionTime;
    }
  }
  return max;
}

int TimerInfo::totalWakeups() const
{
  return m_totalWakeups;
}

QString TimerInfo::state() const
{
  if (!m_timer){
    return QObject::tr("None");
  }

  if (!m_timer->isActive()) {
    return QObject::tr("Inactive");
  } else {
    if (m_timer->isSingleShot()) {
      return QObject::tr("Singleshot (%1 ms)").arg(m_timer->interval());
    } else {
      return QObject::tr("Repeating (%1 ms)").arg(m_timer->interval());
    }
  }
}

void TimerInfo::removeOldEvents()
{
  if (m_timeoutEvents.size() > maxTimeoutEvents) {
    m_timeoutEvents.removeFirst();
  }
}
