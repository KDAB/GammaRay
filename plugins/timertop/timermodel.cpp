
/*
  timermodel.cpp

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
#include "timermodel.h"
#include "functioncalltimer.h"

#include <common/objectmodel.h>
#include <common/objectid.h>

#include <QMutexLocker>
#include <QTimerEvent>
#include <QTime>
#include <QTimer>

#include <QInternal>

#include <iostream>

#define QOBJECT_METAMETHOD(Object, Method) \
    Object::staticMetaObject.method(Object::staticMetaObject.indexOfSlot(#Method))

namespace GammaRay {
struct TimerIdData;
}

using namespace GammaRay;
using namespace std;

static QPointer<TimerModel> s_timerModel;
static const char s_qmlTimerClassName[] = "QQmlTimer";
static QHash<TimerId, TimerIdData> s_gatheredTimersData;
static const int s_maxTimeoutEvents = 1000;
static const int s_maxTimeSpan = 10000;
static QMutex s_mutex;

namespace GammaRay {
struct TimeoutEvent
{
    TimeoutEvent(const QTime &timeStamp = QTime(), int executionTime = -1)
        : timeStamp(timeStamp)
        , executionTime(executionTime)
    { }

    QTime timeStamp;
    int executionTime;
};

struct TimerIdData : TimerIdInfo
{
    TimerIdData()
        : TimerIdInfo()
        , totalWakeupsEvents(0)
    { }

    void update(const TimerId &id, QObject *receiver = nullptr) override
    {
        // If the receiver changed, the timer was stopped / restarted and is no longer the same timer.
        if (id.type() == TimerId::QObjectType &&
                lastReceiverAddress != quintptr(receiver)) {
            clearHistory();
        }

        TimerIdInfo::update(id, receiver);
    }

    void addEvent(const GammaRay::TimeoutEvent &event)
    {
        timeoutEvents.append(event);
        if (timeoutEvents.size() > s_maxTimeoutEvents)
            timeoutEvents.removeFirst();
        totalWakeupsEvents++;
        changed = true;
    }

    void clearHistory()
    {
        TimerIdInfo::totalWakeups = 0;
        TimerIdInfo::lastReceiverAddress = 0;
        TimerIdInfo::lastReceiverObject = nullptr;
        TimerIdInfo::state = (0 << 16) | 0;
        TimerIdInfo::wakeupsPerSec = 0.0;
        TimerIdInfo::timePerWakeup = 0.0;
        TimerIdInfo::maxWakeupTime = 0;

        totalWakeupsEvents = 0;
        if (functionCallTimer.active())
            functionCallTimer.stop();
        timeoutEvents.clear();
        changed = true;
    }

    TimerIdInfo &toInfo(TimerId::Type type)
    {
        TimerIdInfo::totalWakeups =  totalWakeups(type, *this);
        TimerIdInfo::wakeupsPerSec = wakeupsPerSec(type, *this);
        TimerIdInfo::timePerWakeup = timePerWakeup(type, *this);
        TimerIdInfo::maxWakeupTime = maxWakeupTime(type, *this);
        return *this;
    }

    static int totalWakeups(TimerId::Type type, const TimerIdData &data)
    {
        Q_UNUSED(type);
        return data.totalWakeupsEvents;
    }

    static qreal wakeupsPerSec(TimerId::Type type, const TimerIdData &data)
    {
        Q_UNUSED(type);

        int wakeups = 0;
        int start = 0;
        int end = data.timeoutEvents.size() - 1;
        for (int i = end; i >= 0; i--) {
            const TimeoutEvent &event = data.timeoutEvents.at(i);
            if (event.timeStamp.msecsTo(QTime::currentTime()) > s_maxTimeSpan) {
                start = i;
                break;
            }
            wakeups++;
        }

        if (wakeups > 0 && end > start) {
            const QTime startTime = data.timeoutEvents[start].timeStamp;
            const QTime endTime = data.timeoutEvents[end].timeStamp;
            const int timeSpan = startTime.msecsTo(endTime);
            const qreal wakeupsPerSec = wakeups / (qreal)timeSpan * (qreal)1000;
            return wakeupsPerSec;
        }
        return 0;
    }

    static qreal timePerWakeup(TimerId::Type type, const TimerIdData &data)
    {
        if (type == TimerId::QObjectType)
            return 0;

        int wakeups = 0;
        int totalTime = 0;
        for (int i = data.timeoutEvents.size() - 1; i >= 0; i--) {
            const TimeoutEvent &event = data.timeoutEvents.at(i);
            if (event.timeStamp.msecsTo(QTime::currentTime()) > s_maxTimeSpan)
                break;
            wakeups++;
            totalTime += event.executionTime;
        }

        if (wakeups > 0)
            return (qreal)totalTime / (qreal)wakeups;
        return 0;
    }

    static int maxWakeupTime(TimerId::Type type, const TimerIdData &data)
    {
        if (type == TimerId::QObjectType)
            return 0;

        int max = 0;
        for (int i = 0; i < data.timeoutEvents.size(); i++) {
            const TimeoutEvent &event = data.timeoutEvents.at(i);
            if (event.executionTime > max)
                max = event.executionTime;
        }
        return max;
    }

    int totalWakeupsEvents;
    FunctionCallTimer functionCallTimer;
    QList<TimeoutEvent> timeoutEvents;

    bool changed;
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
{
    Q_ASSERT(m_triggerPushChangesMethod.methodIndex() != -1);

    m_pushTimer->setSingleShot(true);
    m_pushTimer->setInterval(5000);
    connect(m_pushTimer, SIGNAL(timeout()), this, SLOT(pushChanges()));

    QInternal::registerCallback(QInternal::EventNotifyCallback, eventNotifyCallback);
}

const TimerIdInfo *TimerModel::findTimerInfo(const QModelIndex &index) const
{
    if (index.row() < m_sourceModel->rowCount()) {
        const QModelIndex sourceIndex = m_sourceModel->index(index.row(), 0);
        QObject * const timerObject = sourceIndex.data(ObjectModel::ObjectRole).value<QObject *>();
        const TimerId id = TimerId(timerObject);
        auto it = m_timersInfo.find(id);

        if (it == m_timersInfo.end()) {
            it = m_timersInfo.insert(id, TimerIdInfo());
            it.value().lockAndUpdate(id);
        }

        return &it.value();
    } else {
        if (index.row() < m_sourceModel->rowCount() + m_freeTimersInfo.count())
            return &m_freeTimersInfo[index.row() - m_sourceModel->rowCount()];
    }

    return nullptr;
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
        Q_ASSERT(timerEvent->timerId() != -1);
        const QTimer *const timer = qobject_cast<QTimer *>(receiver);

        // If there is a QTimer associated with this timer ID, don't handle it here, it will be handled
        // by the signal hooks preSignalActivate/postSignalActivate.
        if (timer && timer->timerId() == timerEvent->timerId()) {
            return false;
        }

        {
            QMutexLocker locker(&s_mutex);
            const TimerId id(timerEvent->timerId());
            auto it = s_gatheredTimersData.find(id);

            if (it == s_gatheredTimersData.end()) {
                it = s_gatheredTimersData.insert(id, TimerIdData());
            }

            const TimeoutEvent timeoutEvent(QTime::currentTime(), -1);

            it.value().lockAndUpdate(id, receiver);
            it.value().addEvent(timeoutEvent);

            s_timerModel->m_triggerPushChangesMethod.invoke(s_timerModel, Qt::QueuedConnection);
        }
    }

    return false;
}

TimerModel::~TimerModel()
{
    QMutexLocker locker(&s_mutex);
    QInternal::unregisterCallback(QInternal::EventNotifyCallback, eventNotifyCallback);
    m_timersInfo.clear();
    s_gatheredTimersData.clear();
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
    // We are in the thread of the caller emmiting the signal
    // The probe did NOT locked the objectLock at this point.
    Q_ASSERT(TimerModel::isInitialized());

    const bool isQTimer = qobject_cast<QTimer *>(caller) != nullptr;
    const bool isQQmlTimer = caller->inherits(s_qmlTimerClassName);

    if (isQQmlTimer && m_qmlTimerTriggeredIndex < 0) {
        m_qmlTimerTriggeredIndex = caller->metaObject()->indexOfMethod("triggered()");
        Q_ASSERT(m_qmlTimerTriggeredIndex != -1);
    }

    if (!(isQTimer && m_timeoutIndex == methodIndex) &&
            !(isQQmlTimer && m_qmlTimerTriggeredIndex == methodIndex))
        return;

    QMutexLocker locker(&s_mutex);
    const TimerId id(caller);
    auto it = s_gatheredTimersData.find(id);

    if (it == s_gatheredTimersData.end()) {
        it = s_gatheredTimersData.insert(id, TimerIdData());
        it.value().lockAndUpdate(id);
    }

    if (!it.value().functionCallTimer.start()) {
        cout << "TimerModel::preSignalActivate(): Recursive timeout for timer "
             << (void *)caller << "!" << endl;
        return;
    }
}

void TimerModel::postSignalActivate(QObject *caller, int methodIndex)
{
    // We are in the thread of the caller emmiting the signal
    // The probe did NOT locked the objectLock at this point
    // and we garanty that caller is still valid.
    Q_UNUSED(methodIndex);
    Q_ASSERT(TimerModel::isInitialized());

    QMutexLocker locker(&s_mutex);
    const TimerId id(caller);
    auto it = s_gatheredTimersData.find(id);

    if (it == s_gatheredTimersData.end()) {
        // A postSignalActivate can be triggered without a preSignalActivate first
        // and/or the caller is not yet gathered.
        return;
    }

    if (!it.value().functionCallTimer.active()) {
        cout << "TimerModel::postSignalActivate(): Timer not active: "
             << (void *)caller << "!" << endl;
        return;
    }

    const TimeoutEvent timeoutEvent(QTime::currentTime(), it.value().functionCallTimer.stop());

    it.value().lockAndUpdate(id);
    it.value().addEvent(timeoutEvent);

    m_triggerPushChangesMethod.invoke(this, Qt::QueuedConnection);
}

void TimerModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    Q_ASSERT(!m_sourceModel);
    beginResetModel();
    m_sourceModel = sourceModel;

    connect(m_sourceModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(slotBeginInsertRows(QModelIndex,int,int)));
    connect(m_sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(slotEndInsertRows()));
    connect(m_sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(slotBeginRemoveRows(QModelIndex,int,int)));
    connect(m_sourceModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(slotEndRemoveRows()));
    connect(m_sourceModel, SIGNAL(modelAboutToBeReset()),
            this, SLOT(slotBeginReset()));
    connect(m_sourceModel, SIGNAL(modelReset()),
            this, SLOT(slotEndReset()));
    connect(m_sourceModel, SIGNAL(layoutAboutToBeChanged()),
            this, SLOT(slotBeginReset()));
    connect(m_sourceModel, SIGNAL(layoutChanged()),
            this, SLOT(slotEndReset()));

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        Q_UNREACHABLE();
#else
        Q_ASSERT(false);
#endif
    }

    return QModelIndex();
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
            return timerInfo->state;
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
    }

    if (index.column() == 0) {
        if (role == TimerModel::ObjectIdRole) {
            const TimerIdInfo *const timerInfo = findTimerInfo(index);

            if (timerInfo) {
                QObject *const object = timerInfo->lastReceiverObject.data();
                Q_ASSERT(index.row() >= m_sourceModel->rowCount() ||
                         object == index.internalPointer());
                return QVariant::fromValue(ObjectId(object));
            }
        }
    }

    return QVariant();
}

QVariant TimerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case ObjectNameColumn:
            return tr("Object Name");
        case StateColumn:
            return tr("State");
        case TotalWakeupsColumn:
            return tr("Total Wakeups");
        case WakeupsPerSecColumn:
            return tr("Wakeups/Sec");
        case TimePerWakeupColumn:
            return tr("Time/Wakeup [uSecs]");
        case MaxTimePerWakeupColumn:
            return tr("Max Wakeup Time [uSecs]");
        case TimerIdColumn:
            return tr("Timer ID");
        case ColumnCount:
            break;
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

QMap<int, QVariant> TimerModel::itemData(const QModelIndex &index) const
{
    auto d = QAbstractTableModel::itemData(index);
    if (index.column() == 0)
        d.insert(TimerModel::ObjectIdRole, QVariant::fromValue(static_cast<QObject *>(index.internalPointer())));
    return d;
}

void TimerModel::clearHistory()
{
    QMutexLocker locker(&s_mutex);
    s_gatheredTimersData.clear();
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
    QMutexLocker locker(&s_mutex);
    TimerIdInfoHash infoHash;

    infoHash.reserve(s_gatheredTimersData.count());
    for (auto it = s_gatheredTimersData.begin(); it != s_gatheredTimersData.end();) {
        // Invalidated during the sync delay, remove entry
        if (!it.value().isValid()) {
            it = s_gatheredTimersData.erase(it);
            continue;
        }

        TimerIdData &itInfo = it.value();

        if (itInfo.changed) {
            infoHash.insert(it.key(), itInfo.toInfo(it.key().type()));
            itInfo.changed = false;
        }

        ++it;
    }
    infoHash.squeeze();

    locker.unlock();
    applyChanges(infoHash);
}

void TimerModel::applyChanges(const GammaRay::TimerIdInfoHash &changes)
{
    QSet<TimerId> updatedIds;
    QVector<QPair<int, int>> dataChangedRanges; // pair of first/last
    QSet<int> qtimerIds;

    // Update QQmlTimer / QTimer entries
    for (int i = 0; i < m_sourceModel->rowCount(); ++i) {
        const QModelIndex idx = m_sourceModel->index(i, 0);
        QObject *const timerObject = idx.data(ObjectModel::ObjectRole).value<QObject *>();
        const TimerId id(timerObject);
        const auto cit = changes.constFind(id);
        auto it = m_timersInfo.find(id);

        if (it == m_timersInfo.end()) {
            it = m_timersInfo.insert(id, TimerIdInfo());

            if (cit == changes.constEnd())
                it.value().lockAndUpdate(id);
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

        // Remember valid QTimer id's for later
        if (it.value().timerId != -1)
            qtimerIds << it.value().timerId;
    }

    // Update existing free timers entries
    for (auto it = m_freeTimersInfo.begin(), end = m_freeTimersInfo.end(); it != end; ++it) {
        const TimerId id((*it).timerId);
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
        // If a TimerId of type TimerId::QObjectType just changed then this free timer id is still valid,
        // remove it from qtimerIds.
        if (it.key().type() == TimerId::QObjectType)
            qtimerIds.remove(it.key().timerId());

        if (updatedIds.contains(it.key()))
            continue;

        // Changes to an object not (yet/longer) in the model, ignore it.
        if (it.key().type() != TimerId::QObjectType)
            continue;

        freeTimersToInsert << it.value();
    }

    // Inform model about new rows
    if (!freeTimersToInsert.isEmpty()) {
        const int first = m_sourceModel->rowCount() + m_freeTimersInfo.count();
        const int last = m_sourceModel->rowCount() + m_freeTimersInfo.count() + freeTimersToInsert.count() - 1;
        beginInsertRows(QModelIndex(), first, last);
        m_freeTimersInfo << freeTimersToInsert;
        endInsertRows();
    }

    // Inform model about data changes
    foreach (const auto &range, dataChangedRanges) {
        emit dataChanged(index(range.first, 0), index(range.second, columnCount() - 1));
    }

    // Check for invalid free timers
    // Invalid QQmlTimer/QTimer are handled in the source model begin remove rows slot already.
    // We considere free timers invalid if an id is used by a qtimer already and free timer id
    // is not part of this changes.
    QVector<QPair<int, int>> removeRowsRanges; // pair of first/last

    for (int row = 0; row < m_freeTimersInfo.count(); ++row) {
        const TimerIdInfo &it = m_freeTimersInfo[row];

        // This is an invalid free timer
        if (qtimerIds.contains(it.timerId)) {
            const int i = m_sourceModel->rowCount() + row;

            if (removeRowsRanges.isEmpty() || removeRowsRanges.last().second != i - 1) {
                removeRowsRanges << qMakePair(i, i);
            } else {
                removeRowsRanges.last().second = i;
            }
        }
    }

    // Inform model about rows removal
    for (int i = removeRowsRanges.count() -1; i >= 0; --i) {
        const auto &range = removeRowsRanges[i];

        beginRemoveRows(QModelIndex(), range.first, range.second);
        m_freeTimersInfo.remove(range.first - m_sourceModel->rowCount(), range.second - range.first + 1);
        endRemoveRows();
    }
}

void TimerModel::slotBeginRemoveRows(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);

    QMutexLocker locker(&s_mutex);

    beginRemoveRows(QModelIndex(), start, end);

    // TODO: Use a delayed timer for that so the hash is iterated once only for a
    // group of successive removal ?
    for (auto it = m_timersInfo.begin(); it != m_timersInfo.end();) {
        if (it.value().isValid()) {
            ++it;
        } else {
            s_gatheredTimersData.remove(it.key());
            it = m_timersInfo.erase(it);
        }
    }

    // TODO: Automatically remove invalidated free timers because of the removal of deleted objects.
}

void TimerModel::slotEndRemoveRows()
{
    endRemoveRows();
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
    QMutexLocker locker(&s_mutex);

    beginResetModel();

    s_gatheredTimersData.clear();
    m_timersInfo.clear();
    m_freeTimersInfo.clear();
}

void TimerModel::slotEndReset()
{
    endResetModel();
}
