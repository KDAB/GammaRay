/*
  eventtypefilter.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    const EventTypeModel *m_eventTypeModel = nullptr;
};
}


#endif // GAMMARAY_EVENTMONITOR_EVENTTYPEFILTER_H
