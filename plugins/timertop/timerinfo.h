/*
  timerinfo.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

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
#ifndef GAMMARAY_TIMERTOP_TIMERINFO_H
#define GAMMARAY_TIMERTOP_TIMERINFO_H

#include "functioncalltimer.h"

#include <QSharedPointer>
#include <QPointer>
#include <QTimer>
#include <QTime>
#include <QMetaType>

namespace GammaRay {
class TimerInfo
{
public:
    enum Type {
        QTimerType,
        QObjectType,
        QQmlTimerType
    };

    struct TimeoutEvent
    {
        QTime timeStamp;
        int executionTime;
    };

    explicit TimerInfo(QObject *timer);
    explicit TimerInfo(int timerId);
    Type type() const;
    void addEvent(const TimeoutEvent &timeoutEvent);
    void setLastReceiver(QObject *receiver);
    int numEvents() const;
    QTimer *timer() const;
    QObject *timerObject() const;
    int timerId() const;
    FunctionCallTimer *functionCallTimer();
    QString wakeupsPerSec() const;
    QString timePerWakeup() const;
    QString maxWakeupTime() const;
    int totalWakeups() const;
    QString state() const;
    QString displayName() const;

private:
    Type m_type;
    int m_totalWakeups;

    // Only for QTimer/QQmlTimers timers
    QPointer<QObject> m_timer;

    int m_timerId;
    FunctionCallTimer m_functionCallTimer;
    QList<TimeoutEvent> m_timeoutEvents;

    // Only for free timers, QObject that received the timeout event
    QPointer<QObject> m_lastReceiver;

    void removeOldEvents();
};

typedef QSharedPointer<TimerInfo> TimerInfoPtr;
}

Q_DECLARE_METATYPE(GammaRay::TimerInfoPtr)

#endif // GAMMARAY_TIMERINFO_H
