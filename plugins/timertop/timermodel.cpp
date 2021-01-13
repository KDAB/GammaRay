
/*
  timermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "timermodel.h"

#include <core/objectdataprovider.h>

#include <common/objectmodel.h>
#include <common/objectid.h>
#include <common/sourcelocation.h>

#include <compat/qasconst.h>

#include <QElapsedTimer>
#include <QMutexLocker>
#include <QTimerEvent>
#include <QTime>
#include <QTimer>
#include <QAbstractEventDispatcher>

#include <QInternal>

#include <iostream>

#define QOBJECT_METAMETHOD(Object, Method) \
    Object::staticMetaObject.method(Object::staticMetaObject.indexOfSlot(#Method))

using namespace GammaRay;
using namespace std;

static QPointer<TimerModel> s_timerModel;
static const char s_qmlTimerClassName[] = "QQmlTimer";
static const int s_maxTimeoutEvents = 1000;
static const int s_maxTimeSpan = 10000;

namespace GammaRay {
struct TimeoutEvent
{
    explicit TimeoutEvent(const QTime &timeStamp = QTime(), int executionTime = -1)
        : timeStamp(timeStamp)
        , executionTime(executionTime)
    { }

    QTime timeStamp;
    int executionTime;
};

struct TimerIdData
{
    TimerIdData() = default;

    void update(const TimerId &id, QObject *receiver = nullptr)
    {
        info.update(id, receiver);
    }

    void addEvent(const GammaRay::TimeoutEvent &event)
    {
        timeoutEvents.append(event);
        if (timeoutEvents.size() > s_maxTimeoutEvents)
            timeoutEvents.removeFirst();
        totalWakeupsEvents++;
        changed = true;
    }

    TimerIdInfo &toInfo(TimerId::Type type)
    {
        info.totalWakeups =  totalWakeups();
        info.wakeupsPerSec = wakeupsPerSec();
        info.timePerWakeup = timePerWakeup(type);
        info.maxWakeupTime = maxWakeupTime(type);
        return info;
    }

    int totalWakeups() const
    {
        return totalWakeupsEvents;
    }

    qreal wakeupsPerSec() const
    {
        int wakeups = 0;
        int start = 0;
        int end = timeoutEvents.size() - 1;
        for (int i = end; i >= 0; i--) {
            const TimeoutEvent &event = timeoutEvents.at(i);
            if (event.timeStamp.msecsTo(QTime::currentTime()) > s_maxTimeSpan) {
                start = i;
                break;
            }
            wakeups++;
        }

        if (wakeups > 0 && end > start) {
            const QTime startTime = timeoutEvents[start].timeStamp;
            const QTime endTime = timeoutEvents[end].timeStamp;
            const int timeSpan = startTime.msecsTo(endTime);
            const qreal wakeupsPerSec = wakeups / (qreal)timeSpan * (qreal)1000;
            return wakeupsPerSec;
        }
        return 0;
    }

    qreal timePerWakeup(TimerId::Type type) const
    {
        if (type == TimerId::QObjectType)
            return 0;

        int wakeups = 0;
        int totalTime = 0;
        for (int i = timeoutEvents.size() - 1; i >= 0; i--) {
            const TimeoutEvent &event = timeoutEvents.at(i);
            if (event.timeStamp.msecsTo(QTime::currentTime()) > s_maxTimeSpan)
                break;
            wakeups++;
            totalTime += event.executionTime;
        }

        if (wakeups > 0)
            return (qreal)totalTime / (qreal)wakeups;
        return 0;
    }

    int maxWakeupTime(TimerId::Type type) const
    {
        if (type == TimerId::QObjectType)
            return 0;

        int max = 0;
        for (auto event : timeoutEvents) {
            if (event.executionTime > max)
                max = event.executionTime;
        }
        return max;
    }

    TimerIdInfo info;
    int totalWakeupsEvents = 0;
    QElapsedTimer functionCallTimer;
    QList<TimeoutEvent> timeoutEvents;

    bool changed = false;
};
}

Q_DECLARE_METATYPE(GammaRay::TimeoutEvent)

TimerModel::TimerModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_sourceModel(nullptr)
    , m_pushTimer(new QTimer(this))
    , m_triggerPushChangesMethod(QOBJECT_METAMETHOD(TimerModel, triggerPushChanges()))
    , m_timeoutIndex(QTimer::staticMetaObject.indexOfSignal("timeout()"))
    , m_qmlTimerTriggeredIndex(-1)
    , m_qmlTimerRunningChangedIndex(-1)
{
    Q_ASSERT(m_triggerPushChangesMethod.methodIndex() != -1);

    m_pushTimer->setSingleShot(true);
    m_pushTimer->setInterval(5000);
    connect(m_pushTimer, &QTimer::timeout, this, &TimerModel::pushChanges);

    QInternal::registerCallback(QInternal::EventNotifyCallback, eventNotifyCallback);
}

const TimerIdInfo *TimerModel::findTimerInfo(const QModelIndex &index) const
{
    if (index.row() < m_sourceModel->rowCount()) {
        const QModelIndex sourceIndex = m_sourceModel->index(index.row(), 0);
        QObject * const timerObject = sourceIndex.data(ObjectModel::ObjectRole).value<QObject *>();

        // The object might have already be deleted even if our index is valid
        if (!timerObject)
            return nullptr;

        const TimerId id = TimerId(timerObject);
        auto it = m_timersInfo.find(id);

        if (it == m_timersInfo.end()) {
            it = m_timersInfo.insert(id, TimerIdInfo());
            // safe, as timerObject has been validated by the source model
            it.value().update(id);
        }

        return &it.value();
    } else {
        if (index.row() < m_sourceModel->rowCount() + m_freeTimersInfo.count())
            return &m_freeTimersInfo[index.row() - m_sourceModel->rowCount()];
    }

    return nullptr;
}

bool TimerModel::canHandleCaller(QObject *caller, int methodIndex) const
{
    const bool isQTimer = qobject_cast<QTimer *>(caller) != nullptr;
    const bool isQQmlTimer = caller->inherits(s_qmlTimerClassName);

    if (isQQmlTimer && m_qmlTimerTriggeredIndex < 0) {
        m_qmlTimerTriggeredIndex = caller->metaObject()->indexOfMethod("triggered()");
        Q_ASSERT(m_qmlTimerTriggeredIndex != -1);
        m_qmlTimerRunningChangedIndex = caller->metaObject()->indexOfMethod("runningChanged()");
        Q_ASSERT(m_qmlTimerRunningChangedIndex != -1);
    }

    return (isQTimer && m_timeoutIndex == methodIndex) ||
            (isQQmlTimer && (m_qmlTimerTriggeredIndex == methodIndex ||
                             m_qmlTimerRunningChangedIndex == methodIndex));
}

void TimerModel::checkDispatcherStatus(QObject *object)
{
    // m_mutex have to be locked!!
    static QHash<QAbstractEventDispatcher *, QElapsedTimer> dispatcherChecks;
    QAbstractEventDispatcher *dispatcher = QAbstractEventDispatcher::instance(object->thread());
    auto it = dispatcherChecks.find(dispatcher);

    if (it == dispatcherChecks.end()) {
        it = dispatcherChecks.insert(dispatcher, QElapsedTimer());
        it.value().start();
    }

    if (it.value().elapsed() < m_pushTimer->interval())
        return;

    for (auto gIt = m_gatheredTimersData.begin(), end = m_gatheredTimersData.end(); gIt != end; ++gIt) {
        QObject *gItObject = gIt.value().info.lastReceiverObject;
        QAbstractEventDispatcher *gItDispatcher = QAbstractEventDispatcher::instance(gItObject ? gItObject->thread() : nullptr);

        if (gItDispatcher != dispatcher) {
            if (!gItObject)
                gIt.value().update(gIt.key(), gItObject);
            continue;
        }

        switch(gIt.key().type()) {
        case TimerId::InvalidType:
        case TimerId::QQmlTimerType:
            continue;
        case TimerId::QTimerType:
        case TimerId::QObjectType:
            break;
        }

        switch(gIt.value().info.state) {
        case TimerIdInfo::InactiveState:
            gIt.value().update(gIt.key(), gItObject);
        case TimerIdInfo::InvalidState:
            continue;
        case TimerIdInfo::SingleShotState:
        case TimerIdInfo::RepeatState:
            break;
        }

        int remaining = -1;

        if (gIt.key().timerId() > -1) {
            remaining = dispatcher->remainingTime(gIt.key().timerId());
        }

        // Timer inactive or invalid, or free timer
        if (remaining == -1 || gIt.key().type() == TimerId::QObjectType)
            gIt.value().update(gIt.key(), gItObject);
    }

    it.value().restart();
}

bool TimerModel::eventNotifyCallback(void *data[])
{
    Q_ASSERT(TimerModel::isInitialized());

    /*
     * data[0] == receiver
     * data[1] == event
     * data[2] == bool result ref, what is returned by caller if this function return true
     * (QCoreApplication::notifyInternal / QCoreApplication::notifyInternal2)
     */
    QObject *receiver = static_cast<QObject *>(data[0]);
    QEvent *event = static_cast<QEvent *>(data[1]);
//    bool *result = static_cast<bool *>(data[2]);

    if (event->type() == QEvent::Timer) {
        const QTimerEvent *const timerEvent = static_cast<QTimerEvent *>(event);
        const QTimer *const timer = qobject_cast<QTimer*>(receiver);

        // If there is a QTimer associated with this timer ID, don't handle it here, it will be handled
        // by the signal hooks preSignalActivate/postSignalActivate.
        if (timer && timer->timerId() == timerEvent->timerId()) {
            return false;
        }

        {
            QMutexLocker locker(&s_timerModel->m_mutex);
            const TimerId id(timerEvent->timerId(), receiver);
            auto it = s_timerModel->m_gatheredTimersData.find(id);

            if (it == s_timerModel->m_gatheredTimersData.end()) {
                it = s_timerModel->m_gatheredTimersData.insert(id, TimerIdData());
            }

            const TimeoutEvent timeoutEvent(QTime::currentTime(), -1);
            // safe, we are called from the receiver thread
            it.value().update(id, receiver);
            it.value().addEvent(timeoutEvent);

            s_timerModel->checkDispatcherStatus(receiver);
            s_timerModel->m_triggerPushChangesMethod.invoke(s_timerModel, Qt::QueuedConnection);
        }
    }

    return false;
}

TimerModel::~TimerModel()
{
    QMutexLocker locker(&m_mutex);
    QInternal::unregisterCallback(QInternal::EventNotifyCallback, eventNotifyCallback);
    m_gatheredTimersData.clear();
    m_timersInfo.clear();
    m_freeTimersInfo.clear();
}

bool TimerModel::isInitialized()
{
    return s_timerModel != nullptr;
}

TimerModel *TimerModel::instance()
{
    if (!s_timerModel)
        s_timerModel = new TimerModel;

    Q_ASSERT(s_timerModel);
    return s_timerModel;
}

void TimerModel::preSignalActivate(QObject *caller, int methodIndex)
{
    // We are in the thread of the caller emitting the signal
    // The probe did NOT locked the objectLock at this point.
    Q_ASSERT(TimerModel::isInitialized());

    if (!canHandleCaller(caller, methodIndex))
        return;

    QMutexLocker locker(&m_mutex);
    const TimerId id(caller);
    auto it = m_gatheredTimersData.find(id);

    if (it == m_gatheredTimersData.end()) {
        it = m_gatheredTimersData.insert(id, TimerIdData());
        // safe, we are called from the receiver thread, before a slot had a chance to delete caller
        it.value().update(id);
    }

    if (methodIndex != m_qmlTimerRunningChangedIndex) {
        if (it.value().functionCallTimer.isValid()) {
            cout << "TimerModel::preSignalActivate(): Recursive timeout for timer "
                 << (void *)caller << "!" << endl;
            return;
        }
        it.value().functionCallTimer.start();
    }
}

void TimerModel::postSignalActivate(QObject *caller, int methodIndex)
{
    // We are in the thread of the caller emitting the signal
    // The probe did unlock the objectLock at this point again but validated caller
    Q_ASSERT(TimerModel::isInitialized());

    if (!canHandleCaller(caller, methodIndex))
        return;

    QMutexLocker locker(&m_mutex);
    const TimerId id(caller);
    auto it = m_gatheredTimersData.find(id);

    if (it == m_gatheredTimersData.end()) {
        // A postSignalActivate can be triggered without a preSignalActivate first
        // and/or the caller is not yet gathered.
        return;
    }

    if (methodIndex != m_qmlTimerRunningChangedIndex) {
        if (!it.value().functionCallTimer.isValid()) {
            cout << "TimerModel::postSignalActivate(): Timer not active: "
                 << (void *)caller << "!" << endl;
            return;
        }
    }

    // safe, nobody in this thread had a chance to delete caller since Probe validated it
    it.value().update(id);

    if (methodIndex != m_qmlTimerRunningChangedIndex) {
        const TimeoutEvent timeoutEvent(QTime::currentTime(), it.value().functionCallTimer.nsecsElapsed() / 1000); // expected unit is µs
        it.value().addEvent(timeoutEvent);
        it.value().functionCallTimer.invalidate();
    }

    checkDispatcherStatus(caller);
    m_triggerPushChangesMethod.invoke(this, Qt::QueuedConnection);
}

void TimerModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    Q_ASSERT(!m_sourceModel);
    beginResetModel();
    m_sourceModel = sourceModel;

    connect(m_sourceModel, &QAbstractItemModel::rowsAboutToBeInserted,
            this, &TimerModel::slotBeginInsertRows);
    connect(m_sourceModel, &QAbstractItemModel::rowsInserted,
            this, &TimerModel::slotEndInsertRows);
    connect(m_sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved,
            this, &TimerModel::slotBeginRemoveRows);
    connect(m_sourceModel, &QAbstractItemModel::rowsRemoved,
            this, &TimerModel::slotEndRemoveRows);
    connect(m_sourceModel, &QAbstractItemModel::modelAboutToBeReset,
            this, &TimerModel::slotBeginReset);
    connect(m_sourceModel, &QAbstractItemModel::modelReset,
            this, &TimerModel::slotEndReset);
    connect(m_sourceModel, &QAbstractItemModel::layoutAboutToBeChanged,
            this, &TimerModel::slotBeginReset);
    connect(m_sourceModel, &QAbstractItemModel::layoutChanged,
            this, &TimerModel::slotEndReset);

    endResetModel();
}

QModelIndex TimerModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        if (row < m_sourceModel->rowCount())  {
            const QModelIndex sourceIndex = m_sourceModel->index(row, 0);
            QObject *const timerObject = sourceIndex.data(ObjectModel::ObjectRole).value<QObject *>();
            return createIndex(row, column, timerObject);
        } else {
            return createIndex(row, column, row - m_sourceModel->rowCount());
        }
    }

    return {};
}

int TimerModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

int TimerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!m_sourceModel || parent.isValid())
        return 0;
    return m_sourceModel->rowCount() + m_freeTimersInfo.count();
}

QVariant TimerModel::data(const QModelIndex &index, int role) const
{
    if (!m_sourceModel || !index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const TimerIdInfo *const timerInfo = findTimerInfo(index);

        if (!timerInfo)
            return QVariant();

        switch (index.column()) {
        case ObjectNameColumn:
            return timerInfo->objectName;
        case StateColumn:
            return int(timerInfo->state);
        case TotalWakeupsColumn:
            return timerInfo->totalWakeups;
        case WakeupsPerSecColumn:
            return timerInfo->wakeupsPerSec;
        case TimePerWakeupColumn:
            return timerInfo->timePerWakeup;
        case MaxTimePerWakeupColumn:
            return timerInfo->maxWakeupTime;
        case TimerIdColumn:
            return timerInfo->timerId;
        case ColumnCount:
            break;
        }
    } else if (role == TimerIntervalRole && index.column() == StateColumn) {
        const TimerIdInfo *const timerInfo = findTimerInfo(index);

        if (timerInfo) {
            return timerInfo->interval;
        }
    } else if (index.column() == 0) {
        auto timerInfo = findTimerInfo(index);
        if (!timerInfo)
            return QVariant();
        auto object = timerInfo->lastReceiverObject.data();
        if (!object)
            return QVariant();

        switch (role) {
            case ObjectModel::ObjectIdRole:
            {
                Q_ASSERT(index.row() >= m_sourceModel->rowCount() || object == index.internalPointer());
                return QVariant::fromValue(ObjectId(object));
            }
            case ObjectModel::CreationLocationRole:
            {
                const auto loc = ObjectDataProvider::creationLocation(object);
                return loc.isValid() ? QVariant::fromValue(loc) : QVariant();
            }
            case ObjectModel::DeclarationLocationRole:
            {
                const auto loc = ObjectDataProvider::declarationLocation(object);
                return loc.isValid() ? QVariant::fromValue(loc) : QVariant();
            }
            case TimerTypeRole:
                return int(timerInfo->type);
        }
    }

    return QVariant();
}

QMap<int, QVariant> TimerModel::itemData(const QModelIndex &index) const
{
    auto d = QAbstractTableModel::itemData(index);
    if (index.column() == 0) {
        d.insert(ObjectModel::ObjectIdRole, index.data(ObjectModel::ObjectIdRole));
        auto v = index.data(ObjectModel::CreationLocationRole);
        if (v.isValid())
            d.insert(ObjectModel::CreationLocationRole, v);
        v = index.data(ObjectModel::DeclarationLocationRole);
        if (v.isValid())
            d.insert(ObjectModel::DeclarationLocationRole, v);
        d.insert(TimerTypeRole, index.data(TimerTypeRole));
    }
    if (index.column() == StateColumn)
        d.insert(TimerModel::TimerIntervalRole, index.data(TimerModel::TimerIntervalRole));
    return d;
}

void TimerModel::clearHistory()
{
    QMutexLocker locker(&m_mutex);
    m_gatheredTimersData.clear();
    locker.unlock();

    const int count = m_sourceModel->rowCount();

    m_timersInfo.clear();

    if (count > 0) {
        const QModelIndex tl = index(0, 0);
        const QModelIndex br = index(count - 1, columnCount() - 1);
        emit dataChanged(tl, br);
    }

    if (!m_freeTimersInfo.isEmpty()) {
        beginRemoveRows(QModelIndex(), m_sourceModel->rowCount(), m_sourceModel->rowCount() + m_freeTimersInfo.count() - 1);
        m_freeTimersInfo.clear();
        endRemoveRows();
    }
}

void TimerModel::triggerPushChanges()
{
    if (!m_pushTimer->isActive())
        m_pushTimer->start();
}

void TimerModel::pushChanges()
{
    QMutexLocker locker(&m_mutex);
    TimerIdInfoContainer changes;
    QSet<int> activeQTimers;

    // TimerId are sort by types matching the TimerId::Type order first
    // so we garranty that free timers checks are done after any qqmltimer/qtimer.
    for (auto it = m_gatheredTimersData.begin(); it != m_gatheredTimersData.end();) {
        TimerIdData &itInfo = it.value();

        // If a free timer did not changed and own an active QTimer id,
        // then make it invalid.
        if (!itInfo.changed) {
            if (it.key().type() == TimerId::QObjectType) {
                if (itInfo.info.state > TimerIdInfo::InactiveState) {
                    if (activeQTimers.contains(itInfo.info.timerId) ||
                            !itInfo.info.lastReceiverObject) {
                        itInfo.info.type = TimerId::InvalidType;
                        itInfo.info.state = TimerIdInfo::InvalidState;
                        itInfo.changed = true;
                    }
                }
            }
        }

        if (itInfo.changed) {
            // If a TimerId of type TimerId::QObjectType just changed,
            // then this free timer id is still valid, remove it from activeQTimers.
            if (it.key().type() == TimerId::QObjectType) {
                if (itInfo.info.state > TimerIdInfo::InactiveState) {
                    if (itInfo.info.lastReceiverObject) {
                        activeQTimers.remove(itInfo.info.timerId);
                    } else {
                        itInfo.info.type = TimerId::InvalidType;
                        itInfo.info.state = TimerIdInfo::InvalidState;
                    }
                }
            }

            changes.insert(it.key(), itInfo.toInfo(it.key().type()));
            itInfo.changed = false;
        }

        // Remember active QTimer id to detect invalid free timers
        if (it.key().type() == TimerId::QTimerType && itInfo.info.state > TimerIdInfo::InactiveState) {
            activeQTimers << itInfo.info.timerId;
        }

        ++it;
    }

    locker.unlock();
    applyChanges(changes);
}

void TimerModel::applyChanges(const TimerIdInfoContainer &changes)
{
    QSet<TimerId> updatedIds;
    QVector<QPair<int, int>> dataChangedRanges; // pair of first/last

    // Update QQmlTimer / QTimer entries
    for (int i = 0; i < m_sourceModel->rowCount(); ++i) {
        const QModelIndex sourceIndex = m_sourceModel->index(i, 0);
        QObject *const timerObject = sourceIndex.data(ObjectModel::ObjectRole).value<QObject *>();

        // The object might have already be deleted even if our index is valid
        if (!timerObject)
            return;

        const TimerId id(timerObject);
        const auto cit = changes.constFind(id);
        auto it = m_timersInfo.find(id);

        if (it == m_timersInfo.end()) {
            it = m_timersInfo.insert(id, TimerIdInfo());

            if (cit == changes.constEnd())
                // safe, validated by the source model
                it.value().update(id);
        }

        if (cit != changes.constEnd()) {
            updatedIds << id;
            it.value() = cit.value();

            if (dataChangedRanges.isEmpty() || dataChangedRanges.last().second != i - 1) {
                dataChangedRanges << qMakePair(i, i);
            } else {
                dataChangedRanges.last().second = i;
            }
        }
    }

    // Update existing free timers entries
    for (auto it = m_freeTimersInfo.begin(), end = m_freeTimersInfo.end(); it != end; ++it) {
        const TimerId id((*it).timerId, (*it).lastReceiverAddress);
        const auto cit = changes.constFind(id);

        if (cit != changes.constEnd()) {
            const int i = m_sourceModel->rowCount() + std::distance(m_freeTimersInfo.begin(), it);
            updatedIds << id;
            (*it) = cit.value();

            if (dataChangedRanges.isEmpty() || dataChangedRanges.last().second != i - 1) {
                dataChangedRanges << qMakePair(i, i);
            } else {
                dataChangedRanges.last().second = i;
            }
        }
    }

    // Fill new free timers to insert
    QVector<TimerIdInfo> freeTimersToInsert;
    freeTimersToInsert.reserve(changes.count() - updatedIds.count());
    for (auto it = changes.constBegin(), end = changes.constEnd(); it != end; ++it) {
        if (updatedIds.contains(it.key()))
            continue;

        // Changes to an object not (yet/longer) in the model, ignore it.
        if (it.key().type() != TimerId::QObjectType)
            continue;

        freeTimersToInsert << it.value();
    }

    // Inform model about data changes
    for (const auto &range : qAsConst(dataChangedRanges)) {
        emit dataChanged(index(range.first, 0), index(range.second, columnCount() - 1));
    }

    // Inform model about new rows
    if (!freeTimersToInsert.isEmpty()) {
        const int first = m_sourceModel->rowCount() + m_freeTimersInfo.count();
        const int last = m_sourceModel->rowCount() + m_freeTimersInfo.count() + freeTimersToInsert.count() - 1;
        beginInsertRows(QModelIndex(), first, last);
        m_freeTimersInfo << freeTimersToInsert;
        endInsertRows();
    }
}

void TimerModel::slotBeginRemoveRows(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);

    QMutexLocker locker(&m_mutex);

    beginRemoveRows(QModelIndex(), start, end);

    // TODO: Use a delayed timer for that so the hash is iterated once only for a
    // group of successive removal ?
    for (auto it = m_timersInfo.begin(); it != m_timersInfo.end();) {
        if (it.value().lastReceiverObject) {
            ++it;
        } else {
            m_gatheredTimersData.remove(it.key());
            it = m_timersInfo.erase(it);
        }
    }
}

void TimerModel::slotEndRemoveRows()
{
    endRemoveRows();

    triggerPushChanges();
}

void TimerModel::slotBeginInsertRows(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(), start, end);
}

void TimerModel::slotEndInsertRows()
{
    endInsertRows();
}

void TimerModel::slotBeginReset()
{
    QMutexLocker locker(&m_mutex);

    beginResetModel();

    m_gatheredTimersData.clear();
    m_timersInfo.clear();
    m_freeTimersInfo.clear();
}

void TimerModel::slotEndReset()
{
    endResetModel();
}
