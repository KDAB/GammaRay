/*
  eventtypefilter.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "eventtypefilter.h"

#include "eventtypemodel.h"
#include "eventmodelroles.h"

using namespace GammaRay;

EventTypeFilter::EventTypeFilter(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void EventTypeFilter::setEventTypeModel(const EventTypeModel *typeModel)
{
    m_eventTypeModel = typeModel;
}

bool EventTypeFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex typeIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    QEvent::Type type = sourceModel()->data(typeIndex, EventModelRole::EventTypeRole).value<QEvent::Type>();
    if (m_eventTypeModel && m_eventTypeModel->isVisible(type)) {
        return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    }
    return false;
}

void EventTypeFilter::sort(int, Qt::SortOrder)
{
    QSortFilterProxyModel::sort(0, Qt::DescendingOrder);
}

bool EventTypeFilter::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    // we want newest event on top, but propagated events in order of occurrence
    if (source_left.parent().isValid() && source_right.parent().isValid()) {
        return source_left.row() > source_right.row();
    }
    return source_left.row() < source_right.row();
}
