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

#include <core/probe.h>
#include <core/util.h>
#include <core/varianthandler.h>

#include <QMetaEnum>
#include <QMutexLocker>

using namespace GammaRay;

EventTypeModel::EventTypeModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    qRegisterMetaType<EventTypeData>();
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
        case Columns::Count:
            return eventTypeData->count;
        case Columns::LoggingStatus:
            return ""; //eventTypeData->loggingEnabled;
        }
    } else if (role == Qt::CheckStateRole) {
        switch (index.column()) {
        case Columns::LoggingStatus:
            return eventTypeData->loggingEnabled ? Qt::Checked : Qt::Unchecked;
        }
    }

    return QVariant();
}

Qt::ItemFlags EventTypeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemFlag::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == Columns::LoggingStatus)
        flags |= Qt::ItemIsUserCheckable;

    return flags;
}

bool EventTypeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() != Columns::LoggingStatus || role != Qt::CheckStateRole)
        return false;

    const auto enabled = value.toInt() == Qt::Checked;
    EventTypeData* eventTypeData = static_cast<EventTypeData*>(index.internalPointer());
    eventTypeData->loggingEnabled = enabled;
    emit dataChanged(index, index);
    return true;
}

QVariant EventTypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case Columns::Type:
            return tr("Type");
        case Columns::Count:
            return tr("Count");
        case Columns::LoggingStatus:
            return tr("Logging");
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
    }
}

void EventTypeModel::enableAll()
{
    for (EventTypeData* eventTypeData: m_data) {
        eventTypeData->loggingEnabled = true;
    }
}

void EventTypeModel::disableAll()
{
    for (EventTypeData* eventTypeData: m_data) {
        eventTypeData->loggingEnabled = false;
    }
}

void EventTypeModel::resetCount()
{
    for (EventTypeData* eventTypeData: m_data) {
        eventTypeData->count = 0;
    }
}

void EventTypeModel::initEventTypes()
{
    m_data[QEvent::Type::MouseMove] = new EventTypeData();
    m_data[QEvent::Type::Timer] = new EventTypeData();
}
