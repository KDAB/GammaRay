/*
  timermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/probeinterface.h>
#include <core/signalspycallbackset.h>
#include <common/objectmodel.h>

#include <QMetaMethod>
#include <QCoreApplication>
#include <QTimerEvent>
#include <QThread>

#include <iostream>

static const char timerInfoPropertyName[] = "GammaRay TimerInfo";

using namespace GammaRay;
using namespace std;

static TimerModel *s_timerModel = 0;

static bool processCallback()
{
  ///TODO: multi-threading support
  if (!TimerModel::isInitialized() || QThread::currentThread() != qApp->thread()) {
    return false;
  }
  return true;
}

static void signal_begin_callback(QObject *caller, int method_index, void **argv)
{
  Q_UNUSED(argv);
  if (!processCallback()) {
    return;
  }

  ///TODO: support threads living in other threads
  if (caller->thread() != qApp->thread()) {
    return;
  }

  TimerModel::instance()->preSignalActivate(caller, method_index);
}

static void signal_end_callback(QObject *caller, int method_index)
{
  // NOTE: here and below the caller may be invalid, e.g. if it was deleted from a slot
  if (!processCallback()) {
    return;
  }

  TimerModel::instance()->postSignalActivate(caller, method_index);
}

TimerModel::TimerModel(QObject *parent)
  : QAbstractTableModel(parent),
    m_sourceModel(0),
    m_probe(0),
    m_pendingChanedRowsTimer(new QTimer(this)),
    m_timeoutIndex(QTimer::staticMetaObject.indexOfSignal("timeout()")),
    m_qmlTimerTriggeredIndex(-1)
{
  m_pendingChanedRowsTimer->setInterval(5000);
  m_pendingChanedRowsTimer->setSingleShot(true);
  connect(m_pendingChanedRowsTimer, SIGNAL(timeout()), this, SLOT(flushEmitPendingChangedRows()));
}

TimerModel::~TimerModel()
{
  s_timerModel = 0;
}

bool TimerModel::isInitialized()
{
  return s_timerModel != 0;
}

TimerModel *TimerModel::instance()
{
  if (!s_timerModel) {
    s_timerModel = new TimerModel;
  }
  Q_ASSERT(s_timerModel);
  return s_timerModel;
}

TimerInfoPtr TimerModel::findOrCreateFreeTimerInfo(int timerId)
{
  // First, return the timer info if it already exists
  foreach (const TimerInfoPtr &freeTimer, m_freeTimers) {
    if (freeTimer->timerId() == timerId) {
      return freeTimer;
    }
  }

  // Create a new free timer, and emit the correct update signals
  TimerInfoPtr timerInfo(new TimerInfo(timerId));
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  m_freeTimers.append(timerInfo);
  endInsertRows();
  return timerInfo;
}

TimerInfoPtr TimerModel::findOrCreateQTimerTimerInfo(QObject *timer)
{
  if (!timer) {
    return TimerInfoPtr();
  }

  QVariant timerInfoVariant = timer->property(timerInfoPropertyName);
  if (!timerInfoVariant.isValid()) {
    const TimerInfoPtr info = TimerInfoPtr(new TimerInfo(timer));
    if (m_qmlTimerTriggeredIndex < 0 && info->type() == TimerInfo::QQmlTimerType) {
      m_qmlTimerTriggeredIndex = timer->metaObject()->indexOfMethod("triggered()");
    }
    timerInfoVariant.setValue(info);
    if (timer->thread() == QThread::currentThread()) // ### FIXME: we shouldn't use setProperty() in the first place...
      timer->setProperty(timerInfoPropertyName, timerInfoVariant);
  }

  const TimerInfoPtr timerInfo = timerInfoVariant.value<TimerInfoPtr>();
  Q_ASSERT(timerInfo->timerObject() == timer);
  return timerInfo;
}

TimerInfoPtr TimerModel::findOrCreateQTimerTimerInfo(int timerId)
{
  for (int row = 0; row < m_sourceModel->rowCount(); row++) {
    const QModelIndex sourceIndex = m_sourceModel->index(row, 0);
    QObject *const timerObject = sourceIndex.data(ObjectModel::ObjectRole).value<QObject*>();
    QTimer * const timer = qobject_cast<QTimer*>(timerObject);
    if (timer && timer->timerId() == timerId) {
      return findOrCreateQTimerTimerInfo(timer);
    }
  }

  return TimerInfoPtr();
}

TimerInfoPtr TimerModel::findOrCreateTimerInfo(const QModelIndex &index)
{
  if (index.row() < m_sourceModel->rowCount()){
    const QModelIndex sourceIndex = m_sourceModel->index(index.row(), 0);
    QObject *const timerObject = sourceIndex.data(ObjectModel::ObjectRole).value<QObject*>();
    return findOrCreateQTimerTimerInfo(timerObject);
  } else {
    const int freeListIndex = index.row() - m_sourceModel->rowCount();
    Q_ASSERT(freeListIndex >= 0);
    if (freeListIndex < m_freeTimers.size()) {
      return m_freeTimers.at(freeListIndex);
    }
  }
  return TimerInfoPtr();
}

int TimerModel::rowFor(QObject *timer)
{
  for (int i = 0; i < rowCount(); i++) {
    const TimerInfoPtr timerInfo = findOrCreateTimerInfo(index(i, 0));
    if (timerInfo && timerInfo->timerObject() == timer) {
      return i;
    }
  }
  return -1;
}

void TimerModel::preSignalActivate(QObject *caller, int methodIndex)
{
  if (!(methodIndex == m_timeoutIndex && qobject_cast<QTimer*>(caller)) &&
      !(methodIndex == m_qmlTimerTriggeredIndex && caller->inherits("QQmlTimer")))
  {
    return;
  }

  const TimerInfoPtr timerInfo = findOrCreateQTimerTimerInfo(caller);

  if (!timerInfo) {
    // Ok, likely a GammaRay timer
    //cout << "TimerModel::preSignalActivate(): Unable to find timer "
    //     << (void*)timer << " (" << timer->objectName().toStdString() << ")!" << endl;
    return;
  }

  if (!timerInfo->functionCallTimer()->start()) {
    cout << "TimerModel::preSignalActivate(): Recursive timeout for timer "
         << (void*)caller << " (" << caller->objectName().toStdString() << ")!" << endl;
    return;
  }

  Q_ASSERT(!m_currentSignals.contains(caller));
  m_currentSignals[caller] = timerInfo;
}

void TimerModel::postSignalActivate(QObject *caller, int methodIndex)
{
  QHash<QObject*, TimerInfoPtr>::iterator it = m_currentSignals.find(caller);
  if (it == m_currentSignals.end()) {
    // Ok, likely a GammaRay timer
    // cout << "TimerModel::postSignalActivate(): Unable to find timer "
    //      << (void*)caller << " (" << caller->objectName().toStdString() << ")!" << endl;
    return;
  }

  const TimerInfoPtr timerInfo = *it;
  Q_ASSERT(timerInfo);

  if (!(timerInfo->type() == TimerInfo::QTimerType && methodIndex == m_timeoutIndex) &&
      !(timerInfo->type() == TimerInfo::QQmlTimerType && methodIndex == m_qmlTimerTriggeredIndex))
  {
    return;
  }

  m_currentSignals.erase(it);

  if (!timerInfo->timerObject()) {
    // timer got killed in a slot
    return;
  }

  Q_ASSERT(caller == timerInfo->timerObject());

  if (!timerInfo->functionCallTimer()->active()) {
    cout << "TimerModel::postSignalActivate(): Timer not active: "
          << (void*)caller << " (" << caller->objectName().toStdString() << ")!" << endl;
    return;
  }

  TimerInfo::TimeoutEvent event;
  event.timeStamp = QTime::currentTime();
  event.executionTime = timerInfo->functionCallTimer()->stop();
  timerInfo->addEvent(event);
  const int row = rowFor(timerInfo->timerObject());
  emitTimerObjectChanged(row);
}

void TimerModel::setProbe(ProbeInterface *probe)
{
  m_probe = probe;

  SignalSpyCallbackSet callbacks;
  callbacks.signalBeginCallback = signal_begin_callback;
  callbacks.signalEndCallback = signal_end_callback;

  probe->registerSignalSpyCallbackSet(callbacks);
}

void TimerModel::setSourceModel(QAbstractItemModel *sourceModel)
{
  Q_ASSERT(!m_sourceModel);
  m_sourceModel = sourceModel;
  qApp->installEventFilter(this);

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
  if (!m_sourceModel || parent.isValid()) {
    return 0;
  }
  return m_sourceModel->rowCount() + m_freeTimers.count();
}

QVariant TimerModel::data(const QModelIndex &index, int role) const
{
  if (!m_sourceModel) {
    return QVariant();
  }

  if (role == Qt::DisplayRole && index.isValid() &&
      index.column() >= 0 && index.column() < columnCount()) {
    const TimerInfoPtr timerInfo = const_cast<TimerModel*>(this)->findOrCreateTimerInfo(index);
    switch ((Roles)(index.column() + FirstRole + 1)) {
      case ObjectNameRole: return timerInfo->displayName();
      case StateRole: return timerInfo->state();
      case TotalWakeupsRole: return timerInfo->totalWakeups();
      case WakeupsPerSecRole: return timerInfo->wakeupsPerSec();
      case TimePerWakeupRole: return timerInfo->timePerWakeup();
      case MaxTimePerWakeupRole: return timerInfo->maxWakeupTime();
      case TimerIdRole: return timerInfo->timerId();
      case FirstRole:
      case LastRole:
        break;
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
      case FirstRole:
      case LastRole:
        break;
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

bool TimerModel::eventFilter(QObject *watched, QEvent *event)
{
  if (event->type() == QEvent::Timer) {

    QTimerEvent * const timerEvent = static_cast<QTimerEvent*>(event);

    // If there is a QTimer associated with this timer ID, don't handle it here, it will be handled
    // by the signal hooks for QTimer::timeout()
    if (findOrCreateQTimerTimerInfo(timerEvent->timerId())) {
      return false;
    }

    // check if object is owned by GammaRay itself
    if (m_probe && m_probe->filterObject(watched)) {
      return false;
    }

    const TimerInfoPtr timerInfo = findOrCreateFreeTimerInfo(timerEvent->timerId());
    TimerInfo::TimeoutEvent timeoutEvent;
    timeoutEvent.timeStamp = QTime::currentTime();
    timeoutEvent.executionTime = -1;
    timerInfo->addEvent(timeoutEvent);

    timerInfo->setLastReceiver(watched);
    emitFreeTimerChanged(m_freeTimers.indexOf(timerInfo));
  }
  return false;
}

void TimerModel::slotBeginRemoveRows(const QModelIndex &parent, int start, int end)
{
  Q_UNUSED(parent);
  flushEmitPendingChangedRows();
  beginRemoveRows(QModelIndex(), start, end);
}

void TimerModel::slotEndRemoveRows()
{
  endRemoveRows();
}

void TimerModel::slotBeginInsertRows(const QModelIndex &parent, int start, int end)
{
  Q_UNUSED(parent);
  flushEmitPendingChangedRows();
  beginInsertRows(QModelIndex(), start, end);
}

void TimerModel::slotEndInsertRows()
{
  endInsertRows();
}

void TimerModel::slotBeginReset()
{
  m_pendingChangedTimerObjects.clear();
  m_pendingChangedFreeTimers.clear();
  beginResetModel();
}

void TimerModel::slotEndReset()
{
  endResetModel();
}

void TimerModel::emitTimerObjectChanged(int row)
{
  if (row < 0 || row >= rowCount())
    return;

  m_pendingChangedTimerObjects.insert(row);
  if (!m_pendingChanedRowsTimer->isActive())
    m_pendingChanedRowsTimer->start();
}

void TimerModel::emitFreeTimerChanged(int row)
{
  if (row < 0 || row >= m_freeTimers.count())
    return;

  m_pendingChangedFreeTimers.insert(row);
  if (!m_pendingChanedRowsTimer->isActive())
    m_pendingChanedRowsTimer->start();
}

void TimerModel::flushEmitPendingChangedRows()
{
  foreach (int row, m_pendingChangedTimerObjects) {
    emit dataChanged(index(row, 0), index(row, LastRole - FirstRole - 2));
  }
  m_pendingChangedTimerObjects.clear();

  foreach (int row, m_pendingChangedFreeTimers) {
    emit dataChanged(index(m_sourceModel->rowCount() + row, 0), index(m_sourceModel->rowCount() + row, LastRole - FirstRole - 2));
  }
  m_pendingChangedFreeTimers.clear();
}
