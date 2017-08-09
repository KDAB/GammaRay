/*
  timerinfo.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        Q_UNREACHABLE();
#else
        Q_ASSERT(false);
#endif
        break;

    case TimerId::QQmlTimerType:
    case TimerId::QTimerType:
        return ::qHash(id.m_timerAddress);

    case TimerId::QObjectType:
        return ::qHash(id.m_timerId);
    }

    return 0;
}
}

TimerId::TimerId()
    : m_type(InvalidType)
    , m_timerAddress(0)
    , m_timerId(-1)
{
}

TimerId::TimerId(QObject *timer)
    : m_type(QQmlTimerType)
    , m_timerAddress(quintptr(timer))
    , m_timerId(-1)
{
    Q_ASSERT(timer);

    if (qobject_cast<QTimer *>(timer)) {
        m_type = QTimerType;
    }
}

TimerId::TimerId(int timerId)
    : m_type(QObjectType)
    , m_timerAddress(0)
    , m_timerId(timerId)
{
    Q_ASSERT(m_timerId != -1);
}

TimerId::Type TimerId::type() const
{
    return m_type;
}

quintptr TimerId::address() const
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        Q_UNREACHABLE();
#else
        Q_ASSERT(false);
#endif
        break;

    case TimerId::QQmlTimerType:
    case TimerId::QTimerType:
        return m_timerAddress == other.m_timerAddress;

    case TimerId::QObjectType:
        return m_timerId == other.m_timerId;
    }

    return false;
}

bool TimerId::operator==(QObject *timer) const
{
    return m_timerAddress == quintptr(timer);
}

bool TimerId::operator==(int timerId) const
{
    return m_timerId == timerId;
}

bool TimerId::operator<(const TimerId &other) const
{
    if (m_timerAddress != 0) {
        if (other.m_timerAddress != 0)
            return m_timerAddress < other.m_timerAddress;
        return other.m_timerId != -1 ? m_timerAddress < quintptr(other.m_timerId) : false;
    } else if (m_timerId != -1) {
        if (other.m_timerId != -1)
            return m_timerId < other.m_timerId;
        return other.m_timerAddress != 0 ? quintptr(m_timerId) < other.m_timerAddress : false;
    }

    Q_ASSERT(false);
    return false;
}

void TimerIdInfo::update(const TimerId &id, QObject *receiver)
{
    // The objectLock() must be locked at this point, if not use lockAndUpdate() then.
    QObject *object = receiver ? receiver : reinterpret_cast<QObject *>(id.address());

    if (!Probe::instance()->isValidObject(object)) {
//        Q_ASSERT(false);
        return;
    }

    switch (id.type()) {
    case TimerId::InvalidType: {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        Q_UNREACHABLE();
#else
        Q_ASSERT(false);
#endif
        break;
    }

    state = InvalidState;
    interval = 0;

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
        lastReceiverAddress = quintptr(object);
        lastReceiverObject = receiver;
        objectName = Util::displayString(object);

        const QAbstractEventDispatcher *dispatcher = QAbstractEventDispatcher::instance(object->thread());
        const QList<QAbstractEventDispatcher::TimerInfo> timers = dispatcher->registeredTimers(object);
        const auto it = std::find_if(timers.constBegin(), timers.constEnd(), [this](const QAbstractEventDispatcher::TimerInfo &timer) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            return timer.timerId == timerId;
#else
            return timer.first == timerId;
#endif
        });

        if (it != timers.constEnd()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            interval = (*it).interval;
#else
            interval = (*it).second;
#endif
            state = RepeatState;
        }

        break;
    }
    }
}

void TimerIdInfo::lockAndUpdate(const TimerId &id, QObject *receiver)
{
    QMutexLocker locker(Probe::objectLock());
    update(id, receiver);
}

bool TimerIdInfo::isValid() const
{
    return !lastReceiverObject.isNull();
}
