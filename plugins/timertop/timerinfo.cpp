/*
  timerinfo.cpp

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

QObject* TimerId::address() const
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
        const QTimer *const timer = qobject_cast<QTimer*>(object);
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
