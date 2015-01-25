/*
  timerinfo.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/util.h>

#include <QObject>

using namespace GammaRay;

static const int maxTimeoutEvents = 1000;
static const int maxTimeSpan = 10000;

TimerInfo::TimerInfo(QObject* timer) :
    m_type(QQmlTimerType),
    m_totalWakeups(0),
    m_timer(timer),
    m_timerId(-1),
    m_lastReceiver(0)
{
  if (QTimer *t = qobject_cast<QTimer*>(timer)) {
    m_type = QTimerType;
    m_timerId = t->timerId();
  }
}

TimerInfo::TimerInfo(int timerId)
  : m_type(QObjectType),
    m_totalWakeups(0),
    m_timerId(timerId)
{
}

TimerInfo::Type TimerInfo::type() const
{
  return m_type;
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

QObject* TimerInfo::timerObject() const
{
  return m_timer;
}

QTimer *TimerInfo::timer() const
{
  if (m_type != QTimerType)
    return 0;
  return qobject_cast<QTimer*>(m_timer);
}

int TimerInfo::timerId() const
{
  return m_timerId;
}

FunctionCallTimer *TimerInfo::functionCallTimer()
{
  return &m_functionCallTimer;
}

QString TimerInfo::wakeupsPerSec() const
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
    return QString::number(wakeupsPerSec, 'f', 1);
  }
  return "0";
}

QString TimerInfo::timePerWakeup() const
{
  if (m_type == QObjectType) {
    return "N/A";
  }

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
    return QString::number(totalTime / (float)totalWakeups, 'f', 1);
  }
  return "N/A";
}

QString TimerInfo::maxWakeupTime() const
{
  if (m_type == QObjectType) {
    return "N/A";
  }

  int max = 0;
  for (int i = 0; i < m_timeoutEvents.size(); i++) {
    const TimeoutEvent &event = m_timeoutEvents.at(i);
    if (event.executionTime > max) {
      max = event.executionTime;
    }
  }
  return QString::number(max);
}

int TimerInfo::totalWakeups() const
{
  return m_totalWakeups;
}

QString TimerInfo::state() const
{
  switch (type()) {
    case QTimerType:
    {
      const QTimer *t = timer();
      if (!t)
        return QObject::tr("None");
      if (!t->isActive())
        return QObject::tr("Inactive");
      if (t->isSingleShot())
        return QObject::tr("Singleshot (%1 ms)").arg(t->interval());
      return QObject::tr("Repeating (%1 ms)").arg(t->interval());
    }
    case QQmlTimerType:
    {
      const QObject *obj = timerObject();
      if (!obj)
        return QObject::tr("None");
      const int interval = obj->property("interval").toInt();
      if (!obj->property("running").toBool())
        return QObject::tr("Inactive (%1 ms)").arg(interval);
      if (obj->property("repeat").toBool())
        return QObject::tr("Repeating (%1 ms)").arg(interval);
      return QObject::tr("Singleshot (%1 ms)").arg(interval);
    }
    case QObjectType:
      return "N/A";
  }

  Q_ASSERT(false);
  return QString();
}

void TimerInfo::removeOldEvents()
{
  if (m_timeoutEvents.size() > maxTimeoutEvents) {
    m_timeoutEvents.removeFirst();
  }
}

void TimerInfo::setLastReceiver(QObject *receiver)
{
    m_lastReceiver = receiver;
}

QString TimerInfo::displayName() const
{
  switch (m_type) {
    case QTimerType:
    case QQmlTimerType:
      return Util::displayString(timerObject());
    case QObjectType:
      if (m_lastReceiver) {
        return Util::displayString(m_lastReceiver);
      } else {
        return QObject::tr("Unknown QObject");
      }
  }

  Q_ASSERT(false);
  return QString();
}
