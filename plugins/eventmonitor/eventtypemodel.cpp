/*
  eventtypemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QMutexLocker>

using namespace GammaRay;

EventTypeModel::EventTypeModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    initEventTypes();
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

    EventTypeData* eventTypeData = static_cast<EventTypeData*>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Columns::Type:
            return VariantHandler::displayString(m_data.keys().at(index.row()));
        case Columns::Value:
            return m_data.keys().at(index.row());
        case Columns::Count:
            return eventTypeData->count;
        }
    } else if (role == Qt::CheckStateRole) {
        switch (index.column()) {
        case Columns::RecordingStatus:
            return eventTypeData->recordingEnabled ? Qt::Checked : Qt::Unchecked;
        case Columns::Visibility:
            return eventTypeData->isVisibleInLog ? Qt::Checked : Qt::Unchecked;
        }
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
    EventTypeData* eventTypeData = static_cast<EventTypeData*>(index.internalPointer());
    if (index.column() == Columns::RecordingStatus)
        eventTypeData->recordingEnabled = enabled;
    else if (index.column() == Columns::Visibility)
        eventTypeData->isVisibleInLog = enabled;
    emit dataChanged(index, index, { Qt::CheckStateRole });
    return true;
}

QVariant EventTypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case Columns::Type:
            return tr("Type");
        case Columns::Value:
            return tr("Value");
        case Columns::Count:
            return tr("Count");
        case Columns::RecordingStatus:
            return tr("Record");
        case Columns::Visibility:
            return tr("Show");
        }
    }

    return QVariant();
}

QModelIndex EventTypeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || column >= columnCount() || parent.isValid())
        return {};

    EventTypeData* item = m_data.value(m_data.keys().at(row));

    return createIndex(row, column, item);
}

void EventTypeModel::increaseCount(QEvent::Type type)
{
    if (m_data.contains(type)) {
        EventTypeData* item = m_data.value(type);
        item->count++;
        QModelIndex index = createIndex(m_data.keys().indexOf(type), Columns::Count, item);
        emit dataChanged(index, index);
    } else {
        beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
        EventTypeData* item = new EventTypeData();
        item->count++;
        m_data[type] = item;
        endInsertRows();
    }
}

void EventTypeModel::resetCounts()
{
    beginResetModel();
    for (EventTypeData* eventTypeData: m_data) {
        eventTypeData->count = 0;
    }
    endResetModel();
}

bool EventTypeModel::isRecording(QEvent::Type type) const
{
    if (m_data.contains(type)) {
        return m_data.value(type)->recordingEnabled;
    }
    return true;
}

void EventTypeModel::recordAll()
{
    beginResetModel();
    for (EventTypeData* eventTypeData: m_data) {
        eventTypeData->recordingEnabled = true;
    }
    endResetModel();
}

void EventTypeModel::recordNone()
{
    beginResetModel();
    for (EventTypeData* eventTypeData: m_data) {
        eventTypeData->recordingEnabled = false;
    }
    endResetModel();
}

void EventTypeModel::showAll()
{
    beginResetModel();
    for (EventTypeData* eventTypeData: m_data) {
        eventTypeData->isVisibleInLog = true;
    }
    endResetModel();
}

void EventTypeModel::showNone()
{
    beginResetModel();
    for (EventTypeData* eventTypeData: m_data) {
        eventTypeData->isVisibleInLog = false;
    }
    endResetModel();
}

void EventTypeModel::initEventTypes()
{
    QMetaEnum e = QMetaEnum::fromType<QEvent::Type>();
    beginInsertRows(QModelIndex(), 0, e.keyCount() - 1);
    for (int i = 0; i < e.keyCount(); ++i) {
         m_data[static_cast<QEvent::Type>(e.value(i))] = new EventTypeData();
    }
    endInsertRows();
}
