/*
  clienttimermodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "clienttimermodel.h"

#include "timerinfo.h"
#include "timermodel.h"

#include <QApplication>
#include <QFont>
#include <QColor>

using namespace GammaRay;

ClientTimerModel::ClientTimerModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

ClientTimerModel::~ClientTimerModel() = default;

QVariant ClientTimerModel::data(const QModelIndex &index, int role) const
{
    if (hasIndex(index.row(), index.column())) {
        if (role == Qt::DisplayRole) {
            switch (static_cast<TimerModel::Columns>(index.column())) {
            case TimerModel::ColumnCount:
                Q_ASSERT(false);
            case TimerModel::ObjectNameColumn:
            case TimerModel::TimerIdColumn:
            case TimerModel::TotalWakeupsColumn:
                // Use source model data
                break;
            case TimerModel::StateColumn:
                return stateToString(QSortFilterProxyModel::data(index, role).toInt(),
                                     QSortFilterProxyModel::data(index, TimerModel::TimerIntervalRole).toInt());
            case TimerModel::WakeupsPerSecColumn:
                return wakeupsPerSecToString(QSortFilterProxyModel::data(index, role).toReal());
            case TimerModel::TimePerWakeupColumn:
                return timePerWakeupToString(QSortFilterProxyModel::data(index, role).toReal());
            case TimerModel::MaxTimePerWakeupColumn:
                return maxWakeupTimeToString(QSortFilterProxyModel::data(index, role).toUInt());
            }
        } else if (role == Qt::ToolTipRole) {
            const QModelIndex sibling = index.sibling(index.row(), TimerModel::ObjectNameColumn);
            const TimerId::Type type = TimerId::Type(sibling.data(TimerModel::TimerTypeRole).toInt());
            switch (type) {
            case TimerId::InvalidType:
                return tr("Invalid");
            case TimerId::QQmlTimerType:
                return tr("QQmlTimer");
            case TimerId::QTimerType:
                return tr("QTimer");
            case TimerId::QObjectType:
                return tr("Free Timer");
            }
        } else if (role == Qt::FontRole) {
            const QModelIndex stateSibling = index.sibling(index.row(), TimerModel::StateColumn);
            const TimerIdInfo::State state = TimerIdInfo::State(QSortFilterProxyModel::data(stateSibling, Qt::DisplayRole).toInt());
            const QModelIndex typeSibling = index.sibling(index.row(), TimerModel::ObjectNameColumn);
            const TimerId::Type type = TimerId::Type(typeSibling.data(TimerModel::TimerTypeRole).toInt());
            QFont font = QApplication::font("QAbstractItemView");
            font.setStrikeOut(type == TimerId::InvalidType || state == TimerIdInfo::InvalidState);
            return QVariant::fromValue(font);
        } else if (role == Qt::BackgroundRole) {
            const QModelIndex sibling = index.sibling(index.row(), TimerModel::ObjectNameColumn);
            const TimerId::Type type = TimerId::Type(sibling.data(TimerModel::TimerTypeRole).toInt());
            switch (type) {
            case TimerId::InvalidType:
                return QColor(255, 0, 0, 80);
            case TimerId::QQmlTimerType:
                return QColor(80, 0, 0, 40);
            case TimerId::QTimerType:
                return QColor(0, 80, 0, 40);
            case TimerId::QObjectType:
                return QColor(0, 0, 80, 40);
            }
        }
    }

    return QSortFilterProxyModel::data(index, role);
}

QVariant ClientTimerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case TimerModel::ObjectNameColumn:
            return tr("Object Name");
        case TimerModel::StateColumn:
            return tr("State");
        case TimerModel::TotalWakeupsColumn:
            return tr("Total Wakeups");
        case TimerModel::WakeupsPerSecColumn:
            return tr("Wakeups/Sec");
        case TimerModel::TimePerWakeupColumn:
            return tr("Time/Wakeup [uSecs]");
        case TimerModel::MaxTimePerWakeupColumn:
            return tr("Max Wakeup Time [uSecs]");
        case TimerModel::TimerIdColumn:
            return tr("Timer ID");
        case TimerModel::ColumnCount:
            break;
        }
    }
    return QSortFilterProxyModel::headerData(section, orientation, role);
}

QString ClientTimerModel::stateToString(int state, int interval)
{
    switch (static_cast<TimerIdInfo::State>(state)) {
    case TimerIdInfo::InvalidState: // None
        return tr("None (%1 ms)").arg(interval);
    case TimerIdInfo::InactiveState: // Not Running
        return tr("Inactive (%1 ms)").arg(interval);
    case TimerIdInfo::SingleShotState: // Single Shot
        return tr("Singleshot (%1 ms)").arg(interval);
    case TimerIdInfo::RepeatState: // Repeat
        return tr("Repeating (%1 ms)").arg(interval);
    }

    return QString();
}

QString ClientTimerModel::wakeupsPerSecToString(qreal value)
{
    return qFuzzyIsNull(value) ? tr("0") : QString::number(value, 'f', 1);
}

QString ClientTimerModel::timePerWakeupToString(qreal value)
{
    return qFuzzyIsNull(value) ? QStringLiteral("N/A") : QString::number(value, 'f', 1);
}

QString ClientTimerModel::maxWakeupTimeToString(uint value)
{
    return value == 0 ? tr("N/A") : QString::number(value);
}
