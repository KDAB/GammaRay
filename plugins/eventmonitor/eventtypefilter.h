/*
  eventtypefilter.h

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

#ifndef GAMMARAY_EVENTMONITOR_EVENTTYPEFILTER_H
#define GAMMARAY_EVENTMONITOR_EVENTTYPEFILTER_H

#include <QSortFilterProxyModel>


namespace GammaRay {

class EventTypeModel;

class EventTypeFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit EventTypeFilter(QObject *parent);
    ~EventTypeFilter() override = default;

    void setEventTypeModel(const EventTypeModel *typeModel);

    void sort(int, Qt::SortOrder) override;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    const EventTypeModel*  m_eventTypeModel = nullptr;
};
}


#endif // GAMMARAY_EVENTMONITOR_EVENTTYPEFILTER_H
