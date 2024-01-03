/*
  launcherwindow.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_LAUNCHERWINDOW_H
#define GAMMARAY_LAUNCHERWINDOW_H

#include "gammaray_launcher_ui_export.h"

#include <QDialog>

namespace GammaRay {
class LaunchOptions;
namespace Ui {
class LauncherWindow;
}

/*! Widget for launching a new process with GammaRay injected. */
class GAMMARAY_LAUNCHER_UI_EXPORT LauncherWindow : public QDialog
{
    Q_OBJECT
public:
    explicit LauncherWindow(QWidget *parent = nullptr);
    ~LauncherWindow() override;

    /// returns all information required to perform the launch/attach
    LaunchOptions launchOptions() const;

    void accept() override;

private slots:
    void tabChanged();
    static void help();

private:
    Ui::LauncherWindow *ui;
};
}

#endif // GAMMARAY_LAUNCHERWINDOW_H
