/*
  timermodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#ifndef GAMMARAY_TIMERTOP_TIMERMODEL_H
#define GAMMARAY_TIMERTOP_TIMERMODEL_H

#include "timerinfo.h"

#include <core/objecttypefilterproxymodel.h>
#include <common/modelroles.h>

#include <QAbstractTableModel>

class QTimer;

namespace GammaRay {

class ProbeInterface;

class TimerModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    virtual ~TimerModel();

    /// @return True in case instance() would return a valid pointer, else false
    static bool isInitialized();

    static TimerModel *instance();

    // For the spy callbacks
    void preSignalActivate(QObject *caller, int methodIndex);
    void postSignalActivate(QObject *caller, int methodIndex);

    enum Roles {
      FirstRole = UserRole + 1,
      ObjectNameRole,
      StateRole,
      TotalWakeupsRole,
      WakeupsPerSecRole,
      TimePerWakeupRole,
      MaxTimePerWakeupRole,
      TimerIdRole,
      LastRole
    };

    /// if set, filters out object owned by the probe
    void setProbe(ProbeInterface *probe);

    void setSourceModel(ObjectTypeFilterProxyModel<QTimer> *sourceModel);

    /* reimp */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /* reimp */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /* reimp */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /* reimp */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /* reimp */
    bool eventFilter(QObject * watched, QEvent * event);

  private slots:
    void slotBeginRemoveRows(const QModelIndex &parent, int start, int end);
    void slotEndRemoveRows();
    void slotBeginInsertRows(const QModelIndex &parent, int start, int end);
    void slotEndInsertRows();
    void slotBeginReset();
    void slotEndReset();

  private:
    explicit TimerModel(QObject *parent = 0);

    // Finds only QTimers based on the timer ID, not free timers.
    TimerInfoPtr findOrCreateQTimerTimerInfo(int timerId);

    // Finds both QTimer and free timers
    TimerInfoPtr findOrCreateTimerInfo(const QModelIndex &index);

    // Finds QTimer timers
    TimerInfoPtr findOrCreateQTimerTimerInfo(QTimer *timer);

    // Finds QObject timers
    TimerInfoPtr findOrCreateFreeTimerInfo(int timerId);

    int rowFor(QTimer *timer) ;

    ObjectTypeFilterProxyModel<QTimer> *m_sourceModel;
    QList<TimerInfoPtr> m_freeTimers;
    ProbeInterface *m_probe;
    // current timer signals that are being processed
    QHash<QObject*, TimerInfoPtr> m_currentSignals;
    // the method index of the timeout() signal of a QTimer
    const int m_timeoutIndex;
};

}

#endif
