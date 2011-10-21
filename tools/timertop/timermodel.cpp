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

#include <QMetaMethod>

#include <iostream>

static const int maxTimeoutEvents = 1000;
static const int maxTimeSpan = 10000;
static const char * timerInfoPropertyName = "GammaRay TimerInfo";

using namespace GammaRay;
using namespace std;

Q_GLOBAL_STATIC(TimerModel, s_timerModel)

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
      if (timer->objectName().toLower().startsWith("gammaray")) {
        return 0;
      }
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
  : QAbstractListModel(parent),
    m_sourceModel(0)
{
}

TimerModel *TimerModel::instance()
{
  return s_timerModel();
}

TimerInfoPtr TimerModel::timerInfoFor(QTimer *timer) const
{
  if (!timer) {
    return TimerInfoPtr();
  }

  QVariant timerInfoVariant = timer->property(timerInfoPropertyName);
  if (!timerInfoVariant.isValid()) {
    timerInfoVariant.setValue(TimerInfoPtr(new TimerInfo(timer)));
    timer->setProperty(timerInfoPropertyName, timerInfoVariant);
  }

  const TimerInfoPtr timerInfo = timerInfoVariant.value<TimerInfoPtr>();
  Q_ASSERT(timerInfo->timer() == timer);
  return timerInfo;
}

TimerInfoPtr TimerModel::timerInfoFor(const QModelIndex &index) const
{
  const QModelIndex sourceIndex = m_sourceModel->index(index.row(), 0);
  QObject *const timerObject = sourceIndex.data(ObjectModel::ObjectRole).value<QObject*>();
  QTimer * const timer = qobject_cast<QTimer*>(timerObject);
  return timerInfoFor(timer);
}

int TimerModel::rowFor(QTimer *timer) const
{
  for (int i = 0; i < rowCount(); i++) {
    const TimerInfoPtr timerInfo = timerInfoFor(index(i, 0));
    if (timerInfo->timer() == timer) {
      return i;
    }
  }
  return -1;
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
  const TimerInfoPtr timerInfo = timerInfoFor(timer);
  if (timerInfo) {
    if (!timerInfo->functionCallTimer()->active()) {
      cout << "TimerModel::postSignalActivate(): Timer not active: "
           << (void*)timer << " (" << timer->objectName().toStdString() << ")!" << endl;
    } else {
      TimeoutEvent event;
      event.timeStamp = QTime::currentTime();
      event.executionTime = timerInfo->functionCallTimer()->stop();
      timerInfo->addEvent(event);
      const int row = rowFor(timer);
      if (row != -1) {
        emit dataChanged(index(row, 0), index(row, columnCount() - 1));
      }
    }
  } else {
    // Ok, likely a GammaRay timer
    //cout << "TimerModel::postSignalActivate(): Unable to find timer "
    //     << (void*)timer << " (" << timer->objectName().toStdString() << ")!" << endl;
  }
}

void TimerModel::setSourceModel(ObjectTypeFilterProxyModel<QTimer> *sourceModel)
{
  Q_ASSERT(!m_sourceModel);
  m_sourceModel = sourceModel;

  QSignalSpyCallbackSet callbacks;
  callbacks.slot_begin_callback = 0;
  callbacks.slot_end_callback = 0;
  callbacks.signal_begin_callback = signal_begin_callback;
  callbacks.signal_end_callback = signal_end_callback;

  qt_register_signal_spy_callbacks(callbacks);

  connect(m_sourceModel, SIGNAL(rowsAboutToBeInserted(QModelIndex, int , int)),
          this, SLOT(slotBeginInsertRows(QModelIndex,int,int)));
  connect(m_sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
          this, SLOT(slotEndInsertRows()));
  connect(m_sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int , int)),
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

  reset();
}

int TimerModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return LastRole - FirstRole - 1;
}

int TimerModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  if (!m_sourceModel) {
    return 0;
  }
  return m_sourceModel->rowCount();
}

QVariant TimerModel::data(const QModelIndex &index, int role) const
{
  if (!m_sourceModel) {
    return QVariant();
  }

  if (role == Qt::DisplayRole && index.isValid() &&
      index.column() >= 0 && index.column() < columnCount()) {
    const TimerInfoPtr timerInfo = timerInfoFor(index);
    switch ((Roles)(index.column() + FirstRole + 1)) {
      case ObjectNameRole: return timerInfo->timer()->objectName();
      case StateRole: return tr("TODO");
      case TotalWakeupsRole: return timerInfo->totalWakeups();
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
      case TotalWakeupsRole: return tr("Total Wakeups");
      case WakeupsPerSecRole: return tr("Wakeups/Sec");
      case TimePerWakeupRole: return tr("Time/Wakeup [uSecs]");
      case MaxTimePerWakeupRole: return tr("Max Wakeup Time [uSecs]");
      case TimerIdRole: return tr("Timer ID");
    }
  }
  return QAbstractListModel::headerData(section, orientation, role);
}

void TimerModel::slotBeginRemoveRows(const QModelIndex &parent, int start, int end)
{
  Q_UNUSED(parent);
  beginRemoveRows(QModelIndex(), start, end);
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
  beginResetModel();
}

void TimerModel::slotEndReset()
{
  endResetModel();
}


TimerInfo::TimerInfo(QTimer *timer)
  : m_timer(timer),
    m_totalWakeups(0)
{
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

int TimerInfo::totalWakeups() const
{
  return m_totalWakeups;
}

void TimerInfo::removeOldEvents()
{
  if (m_timeoutEvents.size() > maxTimeoutEvents) {
    m_timeoutEvents.removeFirst();
  }
}

#include "timermodel.moc"
