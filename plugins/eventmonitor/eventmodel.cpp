/*
  eventmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "eventmodel.h"
#include "eventmodelroles.h"

#include <core/probe.h>
#include <core/util.h>
#include <core/varianthandler.h>

#include <common/objectid.h>

#include <QMetaEnum>
#include <QMutexLocker>
#include <QPoint>
#include <QPointF>
#include <QVariantMap>
#include <QTimer>

using namespace GammaRay;

static const int TopLevelId = std::numeric_limits<int>::max();

EventModel::EventModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_pendingEventTimer(new QTimer(this))
{
    qRegisterMetaType<EventData>();

    m_pendingEventTimer->setSingleShot(true);
    m_pendingEventTimer->setInterval(200);
    connect(m_pendingEventTimer, &QTimer::timeout, this, [this]() {
        Q_ASSERT(!m_pendingEvents.isEmpty());
        beginInsertRows(QModelIndex(), m_events.count(), m_events.count() + m_pendingEvents.size() - 1);
        m_events += m_pendingEvents;
        m_pendingEvents.clear();
        endInsertRows();
    });
}

EventModel::~EventModel() = default;

void EventModel::addEvent(const EventData &event)
{
    m_pendingEvents.push_back(event);
    if (!m_pendingEventTimer->isActive()) {
        m_pendingEventTimer->start();
    }
}

void EventModel::clear()
{
    beginResetModel();
    m_events = QVector<EventData>();
    endResetModel();
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

    if (parent.internalId() == TopLevelId && parent.column() == 0) {
        const EventData &event = m_events.at(parent.row());
        return event.propagatedEvents.size();
    }

    return 0;
}

QVariant EventModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.column() >= columnCount())
        return QVariant();

    bool isPropagatedEvent = index.internalId() != TopLevelId;

    int rootEventIndex = isPropagatedEvent ? int(index.internalId()) : index.row();
    Q_ASSERT(rootEventIndex >= 0 && rootEventIndex < m_events.size());
    const EventData &event = isPropagatedEvent
        ? m_events.at(rootEventIndex).propagatedEvents.at(index.row())
        : m_events.at(rootEventIndex);

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case EventModelColumn::Time:
            return isPropagatedEvent ? "<propagated>" : event.time.toString("hh:mm:ss.zzz");
        case EventModelColumn::Type: {
            const auto s = VariantHandler::displayString(event.type);
            if (!s.isEmpty()) {
                return s;
            }
            return event.type;
        }
        case EventModelColumn::Receiver: {
            QMutexLocker lock(Probe::objectLock());
            if (Probe::instance()->isValidObject(event.receiver)) {
                return Util::displayString(event.receiver);
            }
            return Util::addressToString(event.receiver);
        }
        }
    } else if (role == EventModelRole::AttributesRole) {
        QVariantMap attributesMap;
        for (const QPair<const char *, QVariant> &pair : event.attributes) {
            attributesMap.insert(QString::fromUtf8(pair.first), pair.second);
        }
        return attributesMap;
    } else if (role == EventModelRole::ReceiverIdRole && index.column() == EventModelColumn::Receiver) {
        return QVariant::fromValue(ObjectId(event.receiver));
    } else if (role == EventModelRole::EventTypeRole) {
        return QVariant::fromValue(event.type);
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
        if (row >= m_events.at(parent.row()).propagatedEvents.size())
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

QMap<int, QVariant> EventModel::itemData(const QModelIndex &index) const
{
    auto d = QAbstractItemModel::itemData(index);
    if (index.column() == EventModelColumn::Receiver) {
        d.insert(EventModelRole::ReceiverIdRole, index.data(EventModelRole::ReceiverIdRole));
    }
    return d;
}

bool EventModel::hasEvents() const
{
    return !m_events.empty() || !m_pendingEvents.empty();
}

EventData &EventModel::lastEvent()
{
    if (!m_pendingEvents.empty()) {
        return m_pendingEvents.last();
    }
    return m_events.last();
}
