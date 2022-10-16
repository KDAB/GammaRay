/*
  timertopwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/
#ifndef GAMMARAY_TIMERTOP_TIMERTOPWIDGET_H
#define GAMMARAY_TIMERTOP_TIMERTOPWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

namespace GammaRay {
class TimerTopInterface;
namespace Ui {
class TimerTopWidget;
}

class TimerTopWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TimerTopWidget(QWidget *parent = nullptr);
    ~TimerTopWidget() override;

private slots:
    void contextMenu(QPoint pos);

private:
    QScopedPointer<Ui::TimerTopWidget> ui;
    UIStateManager m_stateManager;
    TimerTopInterface *m_interface;
};

class TimerTopUiFactory : public QObject, public StandardToolUiFactory<TimerTopWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_timertop.json")
};
}

#endif // GAMMARAY_TIMERTOPWIDGET_H
