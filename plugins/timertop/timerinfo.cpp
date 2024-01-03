/*
  timerinfo.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "timerinfo.h"
#include "timermodel.h"

#include <core/util.h>
#include <core/probe.h>

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QAbstractEventDispatcher>
#include <QMutexLocker>

using namespace GammaRay;

namespace GammaRay {
uint qHash(const TimerId &id)
{
    switch (id.m_type) {
    case TimerId::InvalidType:
        Q_UNREACHABLE();
        break;

    case TimerId::QQmlTimerType:
    case TimerId::QTimerType:
        return ::qHash(id.m_timerAddress);

    case TimerId::QObjectType:
        return ::qHash(id.m_timerId) + ::qHash(id.m_timerAddress);
    }

    return 0;
}
}

TimerId::TimerId(QObject *timer)
    : m_type(QQmlTimerType)
    , m_timerAddress(timer)
{
    Q_ASSERT(timer);

    if (qobject_cast<QTimer *>(timer)) {
        m_type = QTimerType;
    }
}

TimerId::TimerId(int timerId, QObject *receiver)
    : m_type(QObjectType)
    , m_timerAddress(receiver)
    , m_timerId(timerId)
{
    Q_ASSERT(m_timerId != -1);
}

TimerId::Type TimerId::type() const
{
    return m_type;
}

QObject *TimerId::address() const
{
    return m_timerAddress;
}

int TimerId::timerId() const
{
    return m_timerId;
}

bool TimerId::operator==(const TimerId &other) const
{
    if (m_type != other.m_type)
        return false;

    switch (m_type) {
    case TimerId::InvalidType:
        Q_UNREACHABLE();
        break;

    case TimerId::QQmlTimerType:
    case TimerId::QTimerType:
        return m_timerAddress == other.m_timerAddress;

    case TimerId::QObjectType:
        return m_timerId == other.m_timerId && m_timerAddress == other.m_timerAddress;
    }

    return false;
}

bool TimerId::operator<(const TimerId &other) const
{
    if (m_type == other.m_type) {
        switch (m_type) {
        case TimerId::InvalidType: {
            Q_UNREACHABLE();
            return false;
        }

        case TimerId::QQmlTimerType:
        case TimerId::QTimerType:
            return m_timerAddress < other.m_timerAddress;

        case TimerId::QObjectType: {
            if (m_timerId == other.m_timerId)
                return m_timerAddress < other.m_timerAddress;
            return m_timerId < other.m_timerId;
        }
        }
    }

    return m_type < other.m_type;
}

void TimerIdInfo::update(const TimerId &id, QObject *receiver)
{
    QObject *object = receiver ? receiver : id.address();

    type = id.type();
    state = InvalidState;

    // The timer became invalid
    if (!object || (lastReceiverAddress == object && !lastReceiverObject)) {
        type = TimerId::InvalidType;
        return;
    }

    interval = 0;

    switch (id.type()) {
    case TimerId::InvalidType: {
        Q_UNREACHABLE();
        break;
    }

    case TimerId::QQmlTimerType: {
        timerId = -1;
        interval = object->property("interval").toInt();
        lastReceiverAddress = id.address();
        lastReceiverObject = object;
        objectName = Util::displayString(object);

        if (!object->property("running").toBool())
            state = InactiveState;
        else if (!object->property("repeat").toBool())
            state = SingleShotState;
        else
            state = RepeatState;

        break;
    }

    case TimerId::QTimerType: {
        const QTimer *const timer = qobject_cast<QTimer *>(object);
        timerId = timer->timerId();
        interval = timer->interval();
        lastReceiverAddress = id.address();
        lastReceiverObject = object;
        objectName = Util::displayString(object);

        if (!timer->isActive())
            state = InactiveState;
        else if (timer->isSingleShot())
            state = SingleShotState;
        else
            state = RepeatState;

        break;
    }

    case TimerId::QObjectType: {
        timerId = id.timerId();
        lastReceiverAddress = object;
        lastReceiverObject = receiver;
        objectName = Util::displayString(object);

        const QAbstractEventDispatcher *dispatcher = QAbstractEventDispatcher::instance(object->thread());
        const QList<QAbstractEventDispatcher::TimerInfo> timers = dispatcher->registeredTimers(object);
        const auto it = std::find_if(timers.constBegin(), timers.constEnd(), [this](const QAbstractEventDispatcher::TimerInfo &timer) {
            return timer.timerId == timerId;
        });

        if (it != timers.constEnd()) {
            interval = (*it).interval;
            state = RepeatState;
        }

        break;
    }
    }
}
