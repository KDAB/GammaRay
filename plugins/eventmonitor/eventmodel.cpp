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

#include <core/probe.h>
#include <core/util.h>
#include <core/varianthandler.h>

#include <QMetaEnum>
#include <QMutexLocker>
#include <QPoint>
#include <QPointF>
#include <QVariantMap>

using namespace GammaRay;

static const int TopLevelId = std::numeric_limits<int>::max();

EventModel::EventModel(QObject *parent)
    : QAbstractItemModel(parent)
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
    if (!parent.isValid())
        return m_events.size();

    if (parent.internalId() == TopLevelId && parent.column() == 0)
        // TODO: add event propagation history as children
        return 0;

    return 0;
}

QVariant EventModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > rowCount() || index.column() > columnCount())
        return QVariant();

    int eventIndex = -1;
    if (index.internalId() == TopLevelId)
        eventIndex = index.row();
    else
        eventIndex = int(index.internalId());
    Q_ASSERT(eventIndex >= 0 && eventIndex < m_events.size());
    const EventData &event = m_events.at(eventIndex);

    if (role == Qt::DisplayRole) {
        if (index.internalId() == TopLevelId) {
            switch (index.column()) {
            case EventModelColumn::Time:
                return event.time.toString("hh:mm:ss.zzz");
            case EventModelColumn::Type:
                return VariantHandler::displayString(event.type);
            case EventModelColumn::Receiver:
            {
                QMutexLocker lock(Probe::objectLock());
                if (Probe::instance()->isValidObject(event.receiver)) {
                    return Util::displayString(event.receiver);
                }
                return Util::addressToString(event.receiver);
            }
            }
        } else {
            // TODO: add event propagation history as children
        }
    } else if (role == EventModelRole::Sort) {
        if (index.internalId() == TopLevelId) {
            switch (index.column()) {
            case EventModelColumn::Time:
                return event.time;
            case EventModelColumn::Type:
                return event.type;
            case EventModelColumn::Receiver:
                return reinterpret_cast<qint64>(event.receiver);  // FIXME: how to sort objects?
            }
        }
    } else if (role == EventModelRole::AttributesRole) {
        if (index.internalId() == TopLevelId) {
            QVariantMap attributesMap;
            for (const QPair<const char *, QVariant>& pair: event.attributes) {
                attributesMap.insert(QString::fromUtf8(pair.first), pair.second);
            }
            return attributesMap;
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
        case EventModelColumn::Receiver:
            return tr("Receiver");
        }
    }

    return QVariant();
}

QModelIndex EventModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || column >= columnCount())
        return {};

    if (parent.isValid()) {
        if (row >= m_events.at(parent.row()).attributes.size())
            return QModelIndex();
        return createIndex(row, column, static_cast<quintptr>(parent.row()));
    }
    return createIndex(row, column, TopLevelId);
}

QModelIndex EventModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || child.internalId() == TopLevelId)
        return {};
    return createIndex(int(child.internalId()), 0, TopLevelId);
}
