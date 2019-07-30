/*
  eventtypefilter.cpp

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
    // we want newest event on top, but propagated events in order of occurence
    if (source_left.parent().isValid() && source_right.parent().isValid()) {
        return source_left.row() > source_right.row();
    }
    return source_left.row() < source_right.row();
}
