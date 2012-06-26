/*
  timermodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "include/probeinterface.h"

#include <private/qobject_p.h> //krazy:exclude=camelcase

#include <QMetaMethod>
#include <QCoreApplication>
#include <QTimerEvent>

#include <iostream>

static const char timerInfoPropertyName[] = "GammaRay TimerInfo";

using namespace GammaRay;
using namespace std;

static TimerModel *s_timerModel = 0;

static QTimer *timer_from_callback(QObject *caller, int method_index)
{
  QTimer * const timer = dynamic_cast<QTimer*>(caller);
  if (timer) {
    QMetaMethod method = timer->metaObject()->method(method_index);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if (method.signature() == QLatin1String("timeout()")) {
#else
    if (method.methodSignature() == QByteArray("timeout()")) {
#endif
      if (timer->objectName().toLower().startsWith(QLatin1String("gammaray"))) {
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
  : QAbstractTableModel(parent),
    m_sourceModel(0),
    m_probe(0)
{
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

TimerInfoPtr TimerModel::findOrCreateQTimerTimerInfo(QTimer *timer)
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
    return findOrCreateQTimerTimerInfo(qobject_cast<QTimer*>(timerObject));
  } else {
    const int freeListIndex = index.row() - m_sourceModel->rowCount();
    Q_ASSERT(freeListIndex >= 0);
    if (freeListIndex < m_freeTimers.size()) {
      return m_freeTimers.at(freeListIndex);
    }
  }
  return TimerInfoPtr();
}

int TimerModel::rowFor(QTimer *timer)
{
  for (int i = 0; i < rowCount(); i++) {
    const TimerInfoPtr timerInfo = findOrCreateTimerInfo(index(i, 0));
    if (timerInfo->timer() == timer) {
      return i;
    }
  }
  return -1;
}

void TimerModel::preSignalActivate(QTimer *timer)
{
  const TimerInfoPtr timerInfo = findOrCreateQTimerTimerInfo(timer);
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
  const TimerInfoPtr timerInfo = findOrCreateQTimerTimerInfo(timer);
  if (timerInfo) {
    if (!timerInfo->functionCallTimer()->active()) {
      cout << "TimerModel::postSignalActivate(): Timer not active: "
           << (void*)timer << " (" << timer->objectName().toStdString() << ")!" << endl;
    } else {
      TimerInfo::TimeoutEvent event;
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

void TimerModel::setProbe(ProbeInterface *probe)
{
  m_probe = probe;
}

void TimerModel::setSourceModel(ObjectTypeFilterProxyModel<QTimer> *sourceModel)
{
  Q_ASSERT(!m_sourceModel);
  m_sourceModel = sourceModel;
  qApp->installEventFilter(this);

  QSignalSpyCallbackSet callbacks;
  callbacks.slot_begin_callback = 0;
  callbacks.slot_end_callback = 0;
  callbacks.signal_begin_callback = signal_begin_callback;
  callbacks.signal_end_callback = signal_end_callback;

  qt_register_signal_spy_callbacks(callbacks);

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

    QTimerEvent * const timerEvent = dynamic_cast<QTimerEvent*>(event);
    Q_ASSERT(timerEvent);

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
  }
  return false;
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

#include "timermodel.moc"
