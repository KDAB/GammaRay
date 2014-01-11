/*
  connectpage.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "connectpage.h"
#include "ui_connectpage.h"
#include "networkdiscoverymodel.h"
#include "clientlauncher.h"

#include <common/endpoint.h>

#include <QProcess>
#include <QSettings>

using namespace GammaRay;

ConnectPage::ConnectPage(QWidget* parent): QWidget(parent), ui(new Ui::ConnectPage)
{
  ui->setupUi(this);

  connect(ui->host, SIGNAL(textChanged(QString)), SIGNAL(updateButtonState()));
  connect(ui->port, SIGNAL(valueChanged(int)), SIGNAL(updateButtonState()));

  NetworkDiscoveryModel* model = new NetworkDiscoveryModel(this);
  ui->instanceView->setModel(model);
  connect(ui->instanceView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(instanceSelected()));

  QSettings settings;
  ui->host->setText(settings.value("Connect/Host", QString()).toString());
  ui->port->setValue(settings.value("Connect/Port", Endpoint::defaultPort()).toInt());
}

ConnectPage::~ConnectPage()
{
}

bool ConnectPage::isValid() const
{
  return !ui->host->text().isEmpty();
}

void ConnectPage::launchClient()
{
  ClientLauncher::launchDetached(ui->host->text(), ui->port->value());
}

void ConnectPage::writeSettings()
{
  QSettings settings;
  settings.setValue("Connect/Host", ui->host->text());
  settings.setValue("Connect/Port", ui->port->value());
}

void ConnectPage::instanceSelected()
{
  const QModelIndexList rows = ui->instanceView->selectionModel()->selectedRows();
  if (rows.size() != 1)
    return;

  ui->host->setText(rows.first().data(NetworkDiscoveryModel::HostNameRole).toString());
  ui->port->setValue(rows.first().data(NetworkDiscoveryModel::PortRole).toInt());
}

