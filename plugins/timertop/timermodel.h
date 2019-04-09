/*
  timermodel.h

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
#ifndef GAMMARAY_TIMERTOP_TIMERMODEL_H
#define GAMMARAY_TIMERTOP_TIMERMODEL_H

#include "timerinfo.h"

#include <common/objectmodel.h>

#include <QAbstractTableModel>
#include <QMap>
#include <QMetaMethod>
#include <QMutex>
#include <QVector>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
struct TimerIdData;

class TimerModel : public QAbstractTableModel
{
    Q_OBJECT
    typedef QMap<TimerId, TimerIdInfo> TimerIdInfoContainer;
    typedef QMap<TimerId, TimerIdData> TimerIdDataContainer;

public:
    ~TimerModel() override;

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
        TimerIntervalRole = ObjectModel::UserRole,
        TimerTypeRole
    };

    void setSourceModel(QAbstractItemModel *sourceModel);

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

public slots:
    void clearHistory();

private slots:
    void triggerPushChanges();
    void pushChanges();
    void applyChanges(const GammaRay::TimerModel::TimerIdInfoContainer &changes);

    void slotBeginRemoveRows(const QModelIndex &parent, int start, int end);
    void slotEndRemoveRows();
    void slotBeginInsertRows(const QModelIndex &parent, int start, int end);
    void slotEndInsertRows();
    void slotBeginReset();
    void slotEndReset();

private:
    explicit TimerModel(QObject *parent = nullptr);

    const TimerIdInfo *findTimerInfo(const QModelIndex &index) const;
    bool canHandleCaller(QObject *caller, int methodIndex) const;
    void checkDispatcherStatus(QObject *object);

    static bool eventNotifyCallback(void *data[]);

    // model data
    QAbstractItemModel *m_sourceModel;
    mutable TimerIdInfoContainer m_timersInfo;
    QVector<TimerIdInfo> m_freeTimersInfo;

    QTimer *m_pushTimer;
    const QMetaMethod m_triggerPushChangesMethod;

    // the method index of the timeout() signal of a QTimer
    const int m_timeoutIndex;
    mutable int m_qmlTimerTriggeredIndex;
    mutable int m_qmlTimerRunningChangedIndex;

    TimerIdDataContainer m_gatheredTimersData;
    QMutex m_mutex; // protects m_gatheredTimersData
};

}

#endif
