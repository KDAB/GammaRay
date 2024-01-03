/*
  eventmonitor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
