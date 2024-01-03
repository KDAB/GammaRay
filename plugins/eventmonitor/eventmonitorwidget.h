/*
  eventmonitorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_EVENTMONITOR_EVENTMONITORWIDGET_H
#define GAMMARAY_EVENTMONITOR_EVENTMONITORWIDGET_H

#include "eventmonitorinterface.h"

#include <ui/tooluifactory.h>

#include <QWidget>

namespace GammaRay {
namespace Ui {
class EventMonitorWidget;
}

class EventMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EventMonitorWidget(QWidget *parent = nullptr);
    ~EventMonitorWidget() override;

private slots:
    void pauseAndResume(bool pause);

private:
    void eventTreeContextMenu(QPoint pos);
    void eventInspectorContextMenu(QPoint pos);

    Ui::EventMonitorWidget *ui;
    EventMonitorInterface *m_interface;
};

class EventMonitorUiFactory : public QObject, public StandardToolUiFactory<EventMonitorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_eventmonitor.json")
};
}

#endif // GAMMARAY_EVENTMONITORWIDGET_H
