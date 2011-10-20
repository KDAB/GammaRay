/*
  timermodel.cpp

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
#include "timermodel.h"

#include <3rdparty/qt/qobject_p_copy.h>
#include <probeinterface.h>

#include <QMetaMethod>

#include <iostream>

static const int maxTimeoutEvents = 1000;
static const int maxTimeSpan = 10000;

using namespace GammaRay;
using namespace std;

Q_GLOBAL_STATIC(TimerModel, s_timerModel);

static QList<TimerInfoPtr> s_timerInfos;

FunctionCallTimer::FunctionCallTimer()
  : m_active(false)
{
}

bool FunctionCallTimer::start()
{
  if (m_active) {
    return false;
  }

  clock_gettime(CLOCK_REALTIME, &m_startTime);
  m_active = true;
  return true;
}

bool FunctionCallTimer::active() const
{
  return m_active;
}

int FunctionCallTimer::stop()
{
  Q_ASSERT(m_active);
  m_active = false;
  timespec endTime;
  clock_gettime(CLOCK_REALTIME, &endTime);
  int elapsed = (endTime.tv_nsec - m_startTime.tv_nsec) / 1000;
  elapsed += (endTime.tv_sec - m_startTime.tv_sec) * 1000000;
  return elapsed;
}

QTimer *timer_from_callback(QObject *caller, int method_index)
{
  QTimer * const timer = dynamic_cast<QTimer*>(caller);
  if (timer) {
    QMetaMethod method = timer->metaObject()->method(method_index);
    if (method.signature() == QLatin1String("timeout()")) {
      return timer;
    }
  }
  return 0;
}

static void signal_begin_callback(QObject *caller, int method_index, void **argv)
{
  Q_UNUSED(argv);
  QTimer * const timer = timer_from_callback(caller, method_index);
  if (timer) {
    TimerModel::instance()->preSignalActivate(timer);
  }
}

static void signal_end_callback(QObject *caller, int method_index)
{
  QTimer * const timer = timer_from_callback(caller, method_index);
  if (timer) {
    TimerModel::instance()->postSignalActivate(timer);
  }
}

TimerModel::TimerModel(QObject *parent)
  : QAbstractListModel(parent)
{
}

TimerModel *TimerModel::instance()
{
  return s_timerModel();
}

void TimerModel::slotRowsRemoved(const QModelIndex &parent, int start, int end)
{
  Q_UNUSED(parent);
  Q_ASSERT(start >= 0 && end >= 0 &&
           start < s_timerInfos.size() && end < s_timerInfos.size());
  int count = end - start + 1;
  beginRemoveRows(QModelIndex(), start, end);
  while (count > 0) {
    s_timerInfos.removeAt(start);
    count--;
  }
  endInsertRows();
  checkConsistency();
}

void TimerModel::slotRowsInserted(const QModelIndex &parent, int start, int end)
{
  Q_UNUSED(parent);
  Q_ASSERT(start >= 0 && end >= 0 &&
           start <= s_timerInfos.size() && end <= s_timerInfos.size());
  beginInsertRows(QModelIndex(), start, end);
  for (int i = start; i <= end; i++) {
    s_timerInfos.insert(i, createTimerInfo(timerAt(i)));
  }
  endInsertRows();
  checkConsistency();
}

void TimerModel::slotReset()
{
  beginResetModel();
  s_timerInfos.clear();
  populateTimerList();
  endResetModel();
}

void TimerModel::populateTimerList()
{
  for (int row = 0; row < m_timerFilter->rowCount(); row++) {
    s_timerInfos.append(createTimerInfo(timerAt(row)));
  }
  checkConsistency();
}

void TimerModel::dumpTimerList() const
{
  cout << "----------------------------- Timers ------------------------------------------" << endl;
  cout << "-------------------------------------------------------------------------------" << endl;
  cout << "id     |    timerId | objectName                                     |  wakeups" << endl;
  cout << "-------|------------|------------------------------------------------|---------" << endl;
  for (int i = 0; i < s_timerInfos.size(); i++) {
    TimerInfoPtr timerInfo = s_timerInfos.at(i);
    const QString formatted = QString("%1 | %2 | %3 | %4")
        .arg(i, 6)
        .arg(timerInfo->timer()->timerId(), 10)
        .arg(timerInfo->timer()->objectName(), 46)
        .arg(timerInfo->numEvents(), 8);
    cout << formatted.toStdString() << endl;
  }
  cout << "-------------------------------------------------------------------------------" << endl;
}

TimerInfoPtr TimerModel::createTimerInfo(QTimer *timer) const
{
  TimerInfoPtr timerInfo(new TimerInfo(timer));
  return timerInfo;
}

QTimer *TimerModel::timerAt(int row) const
{
  const QModelIndex index = m_timerFilter->index(row, 0);
  QObject * const timerObject = index.data(ObjectModel::ObjectRole).value<QObject*>();
  return qobject_cast<QTimer*>(timerObject);
}

TimerInfoPtr TimerModel::timerInfoFor(QTimer *timer) const
{
  const int index = indexOfTimer(timer);
  if (index != -1) {
    return s_timerInfos.at(index);
  } else {
    return TimerInfoPtr();
  }
}

int TimerModel::indexOfTimer(QTimer *timer) const
{
  for (int i = 0; i < s_timerInfos.size(); i++) {
    const TimerInfoPtr cur = s_timerInfos.at(i);
    if (cur->timer() == timer) {
      return i;
    }
  }
  return -1;
}

void TimerModel::checkConsistency() const
{
  Q_ASSERT(s_timerInfos.size() == m_timerFilter->rowCount());
  for (int i = 0; i < s_timerInfos.size(); i++) {
    Q_ASSERT(s_timerInfos[i]->timer() == timerAt(i));
  }
}

void GammaRay::TimerModel::setProbeInterface(ProbeInterface *probe)
{
  Q_ASSERT(!m_timerFilter);
  m_timerFilter.reset(new ObjectTypeFilterProxyModel<QTimer>());
  m_timerFilter->setSourceModel(probe->objectListModel());
  populateTimerList();

  QSignalSpyCallbackSet callbacks;
  callbacks.slot_begin_callback = 0;
  callbacks.slot_end_callback = 0;
  callbacks.signal_begin_callback = signal_begin_callback;
  callbacks.signal_end_callback = signal_end_callback;

  qt_register_signal_spy_callbacks(callbacks);

  connect(m_timerFilter.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
  connect(m_timerFilter.data(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
  connect(m_timerFilter.data(), SIGNAL(layoutChanged()),
          this, SLOT(slotReset()));
  connect(m_timerFilter.data(), SIGNAL(modelReset()),
          this, SLOT(slotReset()));
}

void TimerModel::preSignalActivate(QTimer *timer)
{
  const TimerInfoPtr timerInfo = timerInfoFor(timer);
  if (timerInfo) {
    if (!timerInfo->functionCallTimer()->start()) {
      cout << "TimerModel::preSignalActivate(): Recursive timeout for timer "
           << (void*)timer << " (" << timer->objectName().toStdString() << ")!" << endl;
    }
  } else {
    // Ok, likely a GammaRay timer
    //cout << "TimerModel::preSignalActivate(): Unable to find timer "
    //     << (void*)timer << " (" << timer->objectName().toStdString() << ")!" << endl;
  }
}

void TimerModel::postSignalActivate(QTimer *timer)
{
  const int row = indexOfTimer(timer);
  if (row != -1) {
    const TimerInfoPtr timerInfo = s_timerInfos.at(row);
    if (!timerInfo->functionCallTimer()->active()) {
      cout << "TimerModel::postSignalActivate(): Timer not active: "
           << (void*)timer << " (" << timer->objectName().toStdString() << ")!" << endl;
    } else {
      TimeoutEvent event;
      event.timeStamp = QTime::currentTime();
      event.executionTime = timerInfo->functionCallTimer()->stop();
      timerInfo->addEvent(event);
      emit dataChanged(index(row), index(row));
    }
  } else {
    // Ok, likely a GammaRay timer
    //cout << "TimerModel::postSignalActivate(): Unable to find timer "
    //     << (void*)timer << " (" << timer->objectName().toStdString() << ")!" << endl;
  }
}

int TimerModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return s_timerInfos.size();
}

int TimerModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return LastRole - FirstRole - 1;
}

QVariant TimerModel::data(const QModelIndex &index, int role) const
{
  if (role == Qt::DisplayRole && index.isValid() && index.row() >= 0 &&
      index.row() < s_timerInfos.size() && index.column() >= 0 && index.column() < LastRole) {
    TimerInfoPtr timerInfo = s_timerInfos.at(index.row());
    switch ((Roles)(index.column() + FirstRole + 1)) {
      case ObjectNameRole: return timerInfo->timer()->objectName();
      case StateRole: return tr("TODO");
      case WakeupsPerSecRole: return timerInfo->wakeupsPerSec();
      case TimePerWakeupRole: return tr("TODO");
      case MaxTimePerWakeupRole: return tr("TODO");
      case TimerIdRole: return timerInfo->timer()->timerId();
    }
  }
  return QVariant();
}

QVariant TimerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch ((Roles)(section + FirstRole + 1)) {
      case ObjectNameRole: return tr("Object Name");
      case StateRole: return tr("State");
      case WakeupsPerSecRole: return tr("Wakeups/Sec");
      case TimePerWakeupRole: return tr("Time/Wakeup [uSecs]");
      case MaxTimePerWakeupRole: return tr("Max Wakeup Time [uSecs]");
      case TimerIdRole: return tr("Timer ID");
    }
  }
  return QAbstractListModel::headerData(section, orientation, role);
}

TimerInfo::TimerInfo(QTimer *timer)
  : m_timer(timer)
{
}

void TimerInfo::addEvent(const TimeoutEvent &timeoutEvent)
{
  m_timeoutEvents.append(timeoutEvent);
  removeOldEvents();
}

int TimerInfo::numEvents() const
{
  return m_timeoutEvents.size();
}

QTimer *TimerInfo::timer() const
{
  return m_timer;
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

void TimerInfo::removeOldEvents()
{
  if (m_timeoutEvents.size() > maxTimeoutEvents) {
    m_timeoutEvents.removeFirst();
  }
}

#include "timermodel.moc"
