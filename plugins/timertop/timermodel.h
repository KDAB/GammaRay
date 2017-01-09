/*
  timermodel.h

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
#ifndef GAMMARAY_TIMERTOP_TIMERMODEL_H
#define GAMMARAY_TIMERTOP_TIMERMODEL_H

#include "timerinfo.h"

#include <common/modelroles.h>

#include <QAbstractTableModel>
#include <QSet>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {

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

    enum Columns {
        ObjectNameColumn,
        StateColumn,
        TotalWakeupsColumn,
        WakeupsPerSecColumn,
        TimePerWakeupColumn,
        MaxTimePerWakeupColumn,
        TimerIdColumn,
        ColumnCount
    };

    enum Roles {
        ObjectIdRole = UserRole + 1
    };

    void setSourceModel(QAbstractItemModel *sourceModel);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;


    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void slotBeginRemoveRows(const QModelIndex &parent, int start, int end);
    void slotEndRemoveRows();
    void slotBeginInsertRows(const QModelIndex &parent, int start, int end);
    void slotEndInsertRows();
    void slotBeginReset();
    void slotEndReset();
    void flushEmitPendingChangedRows();

private:
    explicit TimerModel(QObject *parent = nullptr);

    // Finds only QTimers based on the timer ID, not free timers.
    TimerInfoPtr findOrCreateQTimerTimerInfo(int timerId);

    // Finds both QTimer and free timers
    TimerInfoPtr findOrCreateTimerInfo(const QModelIndex &index);

    // Finds QTimer timers
    TimerInfoPtr findOrCreateQTimerTimerInfo(QObject *timer);

    // Finds QObject timers
    TimerInfoPtr findOrCreateFreeTimerInfo(int timerId);

    int rowFor(QObject *timer);
    void emitTimerObjectChanged(int row);
    void emitFreeTimerChanged(int row);

    QAbstractItemModel *m_sourceModel;
    QList<TimerInfoPtr> m_freeTimers;
    // current timer signals that are being processed
    QHash<QObject *, TimerInfoPtr> m_currentSignals;
    // pending dataChanged() signals
    QSet<int> m_pendingChangedTimerObjects;
    QSet<int> m_pendingChangedFreeTimers;
    QTimer *m_pendingChanedRowsTimer;
    // the method index of the timeout() signal of a QTimer
    const int m_timeoutIndex;
    int m_qmlTimerTriggeredIndex;
};
}

#endif
