/*
  kjobtrackerwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_KJOBTRACKER_KJOBTRACKERWIDGET_H
#define GAMMARAY_KJOBTRACKER_KJOBTRACKERWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QWidget>

namespace GammaRay {
namespace Ui {
class KJobTrackerWidget;
}

class KJobTrackerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KJobTrackerWidget(QWidget *parent = nullptr);
    ~KJobTrackerWidget() override;

private:
    QScopedPointer<Ui::KJobTrackerWidget> ui;
    UIStateManager m_stateManager;
};

class KJobTrackerUiFactory : public QObject, public StandardToolUiFactory<KJobTrackerWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_kjobtracker.json")
};
}

#endif // GAMMARAY_KJOBTRACKERWIDGET_H
