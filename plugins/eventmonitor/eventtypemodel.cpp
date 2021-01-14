/*
  eventtypemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2019-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tim Henning <tim.henning@kdab.com>

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

#include "eventtypemodel.h"

#include <core/varianthandler.h>

#include <QMetaEnum>
#include <QTimer>

using namespace GammaRay;

EventTypeModel::EventTypeModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_pendingUpdateTimer(new QTimer(this))
{
    initEventTypes();

    m_pendingUpdateTimer->setSingleShot(true);
    m_pendingUpdateTimer->setInterval(500);
    connect(m_pendingUpdateTimer, &QTimer::timeout, this, &EventTypeModel::emitPendingUpdates);
}

EventTypeModel::~EventTypeModel() = default;

int EventTypeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return EventTypeModel::Columns::COUNT;
}

int EventTypeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_data.size();

    return 0;
}

QVariant EventTypeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount() || index.column() >= columnCount())
        return QVariant();

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Columns::Type:
        {
            const auto s = VariantHandler::displayString(m_data[index.row()].type);
            if (s.isEmpty()) {
                return m_data[index.row()].type;
            }
            return QString(s + QLatin1String(" [") + QString::number(m_data[index.row()].type) + QLatin1Char(']'));
        }
        case Columns::Count:
            return m_data[index.row()].count;
        }
    } else if (role == Qt::CheckStateRole) {
        switch (index.column()) {
        case Columns::RecordingStatus:
            return m_data[index.row()].recordingEnabled ? Qt::Checked : Qt::Unchecked;
        case Columns::Visibility:
            return m_data[index.row()].isVisibleInLog ? Qt::Checked : Qt::Unchecked;
        }
    } else if (role == Role::MaxEventCount) {
        return m_maxEventCount;
    }

    return QVariant();
}

Qt::ItemFlags EventTypeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemFlag::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == Columns::RecordingStatus || index.column() == Columns::Visibility)
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}

bool EventTypeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::CheckStateRole
            || (index.column() != Columns::RecordingStatus
                && index.column() != Columns::Visibility))
        return false;

    const auto enabled = value.toInt() == Qt::Checked;
    if (index.column() == Columns::RecordingStatus) {
        m_data[index.row()].recordingEnabled = enabled;
    } else if (index.column() == Columns::Visibility) {
        m_data[index.row()].isVisibleInLog = enabled;
        emit typeVisibilityChanged();
    }
    emit dataChanged(index, index, { Qt::CheckStateRole });
    return true;
}

QMap<int, QVariant> EventTypeModel::itemData(const QModelIndex& index) const
{
    auto d = QAbstractItemModel::itemData(index);
    if (index.column() == Columns::Count) {
        d.insert(Role::MaxEventCount, index.data(Role::MaxEventCount));
    }
    return d;
}

void EventTypeModel::increaseCount(QEvent::Type type)
{
    const auto it = std::lower_bound(m_data.begin(), m_data.end(), type);
    if (it != m_data.end() && (*it).type == type) {
        (*it).count++;
        m_maxEventCount = std::max((*it).count, m_maxEventCount);
        m_pendingUpdates.insert(type);
        if (!m_pendingUpdateTimer->isActive()) {
            m_pendingUpdateTimer->start();
        }
    } else {
        const auto row = std::distance(m_data.begin(), it);
        beginInsertRows(QModelIndex(), row, row);
        EventTypeData item;
        item.type = type;
        item.count++;
        m_maxEventCount = std::max(item.count, m_maxEventCount);
        m_data.insert(it, item);
        endInsertRows();
    }
}

void EventTypeModel::resetCounts()
{
    beginResetModel();
    for (auto &eventTypeData: m_data) {
        eventTypeData.count = 0;
    }
    m_maxEventCount = 0;
    endResetModel();
}

bool EventTypeModel::isRecording(QEvent::Type type) const
{
    const auto it = std::lower_bound(m_data.begin(), m_data.end(), type);
    if (it != m_data.end() && (*it).type == type) {
        return (*it).recordingEnabled;
    }
    return true;
}

void EventTypeModel::recordAll()
{
    beginResetModel();
    for (auto &eventTypeData: m_data) {
        eventTypeData.recordingEnabled = true;
    }
    endResetModel();
}

void EventTypeModel::recordNone()
{
    beginResetModel();
    for (auto &eventTypeData: m_data) {
        eventTypeData.recordingEnabled = false;
    }
    endResetModel();
}

bool EventTypeModel::isVisible(QEvent::Type type) const
{
    const auto it = std::lower_bound(m_data.begin(), m_data.end(), type);
    if (it != m_data.end() && (*it).type == type) {
        return (*it).isVisibleInLog;
    }
    return true;
}

void EventTypeModel::showAll()
{
    beginResetModel();
    for (auto &eventTypeData: m_data) {
        eventTypeData.isVisibleInLog = true;
    }
    endResetModel();
    emit typeVisibilityChanged();
}

void EventTypeModel::showNone()
{
    beginResetModel();
    for (auto &eventTypeData: m_data) {
        eventTypeData.isVisibleInLog = false;
    }
    endResetModel();
    emit typeVisibilityChanged();
}

void EventTypeModel::initEventTypes()
{
    QMetaEnum e = QMetaEnum::fromType<QEvent::Type>();
    beginInsertRows(QModelIndex(), 0, e.keyCount() - 1);
    m_data.reserve(e.keyCount());
    for (int i = 0; i < e.keyCount(); ++i) {
        EventTypeData ev;
        ev.type = static_cast<QEvent::Type>(e.value(i));
        if (ev.type == QEvent::MetaCall) { // expensive to record types are off by default
            ev.recordingEnabled = false;
        }
        m_data.push_back(ev);
    }
    std::sort(m_data.begin(), m_data.end());
    endInsertRows();
}

void EventTypeModel::emitPendingUpdates()
{
    for (auto type : m_pendingUpdates) {
        const auto it = std::lower_bound(m_data.begin(), m_data.end(), static_cast<QEvent::Type>(type));
        Q_ASSERT(it != m_data.end());
        const auto row = std::distance(m_data.begin(), it);
        const auto idx = createIndex(row, EventTypeModel::Columns::Count);
        emit dataChanged(idx, idx);
    }
    m_pendingUpdates.clear();
}
