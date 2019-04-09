/*
  timerinfo.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QPointer>
#include <QHash>
#include <QMetaType>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
class TimerId
{
    friend uint qHash(const TimerId &);

public:
    enum Type {
        InvalidType,
        // A QObject, no timer id
        QQmlTimerType,
        // A QObject, unstable timerId across stop/start
        QTimerType,
        // A timerId attached to a QObject
        QObjectType
    };

    TimerId() = default;
    explicit TimerId(QObject *timer);
    explicit TimerId(int timerId, QObject *receiver);

    Type type() const;
    QObject* address() const;
    int timerId() const;

    bool operator==(const TimerId &other) const;
    bool operator<(const TimerId &other) const;

private:
    Type m_type = InvalidType;
    QObject *m_timerAddress = nullptr;
    int m_timerId = -1;
};

struct TimerIdInfo
{
    enum State {
        InvalidState,
        InactiveState,
        SingleShotState,
        RepeatState
    };

    TimerIdInfo()
        : type(TimerId::InvalidType)
        , timerId(-1)
        , interval(0)
        , totalWakeups(0)
        , lastReceiverAddress(nullptr)
        , state(InvalidState)
        , wakeupsPerSec(0.0)
        , timePerWakeup(0.0)
        , maxWakeupTime(0)
    { }

    ~TimerIdInfo() { }

    /** Update timer information to the current state.
     *  It's the callers responsibility to ensure the receiver or timer object
     *  is and stays valid during this call, if necessary by using Probe::objectLock().
     */
    void update(const TimerId &id, QObject *receiver = nullptr);

    TimerId::Type type;
    int timerId;
    int interval;

    uint totalWakeups;
    QObject* lastReceiverAddress; // The QTimer/QQmlTimer or last known receiver address
    QPointer<QObject> lastReceiverObject;

    QString objectName;
    State state;
    qreal wakeupsPerSec;
    qreal timePerWakeup;
    uint maxWakeupTime;
};

uint qHash(const TimerId &id);
}

#endif // GAMMARAY_TIMERINFO_H
