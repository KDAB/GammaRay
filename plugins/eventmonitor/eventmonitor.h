/*
  eventmonitor.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_EVENTMONITOR_EVENTMONITOR_H
#define GAMMARAY_EVENTMONITOR_EVENTMONITOR_H

#include <core/toolfactory.h>
#include "eventmonitorinterface.h"

#include <QObject>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE


namespace GammaRay {
class AggregatedPropertyModel;
struct EventData;
class EventModel;
class EventTypeModel;


class EventPropagationListener : public QObject
{
    Q_OBJECT

public:
    explicit EventPropagationListener(QObject *parent);

    virtual bool eventFilter(QObject *receiver, QEvent *event) override;
};


class EventMonitor : public EventMonitorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::EventMonitorInterface)

public:
    explicit EventMonitor(Probe *probe, QObject *parent = nullptr);
    ~EventMonitor() override;

public slots:
    void clearHistory() override;
    void recordAll() override;
    void recordNone() override;
    void showAll() override;
    void showNone() override;

    void addEvent(const GammaRay::EventData &event);


private slots:
    void eventSelected(const QItemSelection &selection);

private:
    EventModel *m_eventModel;
    EventTypeModel *m_eventTypeModel;
    AggregatedPropertyModel *m_eventPropertyModel;
};


class EventMonitorFactory : public QObject, public StandardToolFactory<QObject, EventMonitor>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_eventmonitor.json")

public:
    explicit EventMonitorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_EVENTMONITOR_H
