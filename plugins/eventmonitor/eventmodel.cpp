/*
  eventmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "eventmodel.h"
#include "eventmodelroles.h"

#include <QMetaEnum>

using namespace GammaRay;

EventModel::EventModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    qRegisterMetaType<EventData>();
}

EventModel::~EventModel() = default;

void EventModel::addEvent(const EventData &event)
{
    beginInsertRows(QModelIndex(), m_events.count(), m_events.count());
    m_events << event;
    endInsertRows();
}

int EventModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return EventModelColumn::COUNT;
}

int EventModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_events.count();
}

QVariant EventModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount())
        return QVariant();

    const EventData &event = m_events.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case EventModelColumn::Time:
            return event.time.toString();
        case EventModelColumn::Type:
        {
            static int eventEnumIndex = QEvent::staticMetaObject.indexOfEnumerator("Type");
            QString eventTypeName = QEvent::staticMetaObject.enumerator(eventEnumIndex).valueToKey(event.type);
            return eventTypeName.isEmpty() ? QString::number(event.type) : eventTypeName;
        }
        case EventModelColumn::Spontaneous:
            return event.spontaneous;
        case EventModelColumn::Accepted:
            return event.accepted;
        case EventModelColumn::Receiver:
            return reinterpret_cast<qint64>(event.receiver);  // FIXME: show link to object
        }
    } else if (role == EventModelRole::Sort) {
        switch (index.column()) {
        case EventModelColumn::Time:
            return event.time;
        case EventModelColumn::Type:
            return event.type;
        case EventModelColumn::Spontaneous:
            return event.spontaneous;
        case EventModelColumn::Accepted:
            return event.accepted;
        case EventModelColumn::Receiver:
            return reinterpret_cast<qint64>(event.receiver);  // FIXME: how to sort objects?
        }
    }

    return QVariant();
}

QVariant EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case EventModelColumn::Time:
            return tr("Time");
        case EventModelColumn::Type:
            return tr("Type");
        case EventModelColumn::Spontaneous:
            return tr("Spontaneous");
        case EventModelColumn::Accepted:
            return tr("Accepted");
        case EventModelColumn::Receiver:
            return tr("Receiver");
        }
    }

    return QVariant();
}
