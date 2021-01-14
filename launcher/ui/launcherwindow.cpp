/*
  launcherwindow.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "launcherwindow.h"
#include "ui_launcherwindow.h"

#include <launcher/core/launchoptions.h>

#include <ui/aboutdata.h>
#include <ui/helpcontroller.h>
#include <ui/uiresources.h>
#include <ui/uiintegration.h>

#include <QPushButton>
#include <QSettings>

using namespace GammaRay;

LauncherWindow::LauncherWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LauncherWindow)
{
    UIResources::setTheme(UiIntegration::hasDarkUI()
                              ? UIResources::Light
                              : UIResources::Dark);

    ui->setupUi(this);
    ui->aboutPage->setThemeLogo(QStringLiteral("gammaray-trademark.png"));
    ui->aboutPage->setTitle(AboutData::aboutTitle());
    ui->aboutPage->setHeader(AboutData::aboutHeader());
    ui->aboutPage->setAuthors(AboutData::aboutAuthors());
    ui->aboutPage->setFooter(AboutData::aboutFooter());
    ui->aboutPage->layout()->setContentsMargins(ui->selfTestPage->layout()->contentsMargins());
    ui->aboutPage->setBackgroundWindow(this);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &LauncherWindow::tabChanged);
    connect(ui->attachPage, &AttachDialog::updateButtonState, this, &LauncherWindow::tabChanged);
    connect(ui->launchPage, &LaunchPage::updateButtonState, this, &LauncherWindow::tabChanged);
    connect(ui->connectPage, &ConnectPage::updateButtonState, this, &LauncherWindow::tabChanged);
    connect(ui->attachPage, &AttachDialog::activate,
            ui->buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::click);
    connect(ui->connectPage, &ConnectPage::activate,
            ui->buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::click);
    connect(ui->buttonBox, &QDialogButtonBox::helpRequested, this, &LauncherWindow::help);

    setWindowTitle(tr("GammaRay Launcher"));
    ui->buttonBox->button(QDialogButtonBox::Help)->setEnabled(HelpController::isAvailable());
    ui->buttonBox->button(QDialogButtonBox::Help)->setShortcut(QKeySequence::HelpContents);

    QSettings settings;
    ui->tabWidget->setCurrentIndex(settings.value(QStringLiteral("Launcher/TabIndex")).toInt());
}

LauncherWindow::~LauncherWindow()
{
    delete ui;
}

LaunchOptions LauncherWindow::launchOptions() const
{
    QWidget *current = ui->tabWidget->currentWidget();
    if (current == ui->launchPage)
        return ui->launchPage->launchOptions();
    else if (current == ui->attachPage)
        return ui->attachPage->launchOptions();
    return LaunchOptions();
}

void LauncherWindow::tabChanged()
{
    if (ui->tabWidget->currentWidget() == ui->attachPage) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Attach"));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui->attachPage->isValid());
    } else if (ui->tabWidget->currentWidget() == ui->launchPage) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Launch"));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui->launchPage->isValid());
    } else if (ui->tabWidget->currentWidget() == ui->connectPage) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Connect"));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(ui->connectPage->isValid());
    } else {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

void LauncherWindow::accept()
{
    QSettings settings;
    settings.setValue(QStringLiteral("Launcher/TabIndex"), ui->tabWidget->currentIndex());

    ui->launchPage->writeSettings();
    ui->attachPage->writeSettings();
    ui->connectPage->writeSettings();

    if (ui->tabWidget->currentWidget() == ui->connectPage)
        ui->connectPage->launchClient();

    QDialog::accept();
}

void LauncherWindow::help()
{
    HelpController::openPage("gammaray/gammaray-launcher-gui.html");
}
