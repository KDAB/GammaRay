/*
  timerinfo.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    enum Type
    {
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
    QObject *address() const;
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
    enum State
    {
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
    {
    }

    ~TimerIdInfo()
    {
    }

    /** Update timer information to the current state.
     *  It's the callers responsibility to ensure the receiver or timer object
     *  is and stays valid during this call, if necessary by using Probe::objectLock().
     */
    void update(const TimerId &id, QObject *receiver = nullptr);

    TimerId::Type type;
    int timerId;
    int interval;

    uint totalWakeups;
    QObject *lastReceiverAddress; // The QTimer/QQmlTimer or last known receiver address
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
