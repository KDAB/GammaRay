/*
  launcherdialog.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

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

#include <QDialog>

#include "launcherdialog.h"
#include "launcherwindow.h"
#include "ui_launcherwindow.h"

namespace GammaRay {

LauncherDialog::Result LauncherDialog::exec(Mode mode)
{
    LauncherWindow launcher(false);
    launcher.ui->tabWidget->removeTab(launcher.ui->tabWidget->indexOf(launcher.ui->launchPage));

    switch (mode) {
        case Mode::Connect:
            launcher.ui->tabWidget->setCurrentWidget(launcher.ui->connectPage);
            break;
        case Mode::Attach:
            launcher.ui->tabWidget->setCurrentWidget(launcher.ui->attachPage);
            break;
    }

    int ret = launcher.exec();
    Result result;
    result.m_valid = ret == QDialog::Accepted;

    QWidget *current = launcher.ui->tabWidget->currentWidget();
    if (current == launcher.ui->connectPage) {
        result.m_mode = Mode::Connect;
    } else if (current == launcher.ui->attachPage) {
        result.m_mode = Mode::Attach;
    } else {
        result.m_valid = false;
        return result;
    }

    switch (result.m_mode) {
        case Mode::Connect:
            result.m_url = launcher.ui->connectPage->url();
            break;
        case Mode::Attach:
            result.m_procPid = launcher.ui->attachPage->pid();
            result.m_procExe = launcher.ui->attachPage->name();
            break;
    }


    return result;
}

}
