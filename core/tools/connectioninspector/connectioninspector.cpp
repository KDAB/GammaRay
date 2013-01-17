/*
  connectioninspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "connectioninspector.h"
#include "connectionfilterproxymodel.h"
#include "proxydetacher.h"
#include "ui_connectioninspector.h"

#include "include/probeinterface.h"

#include <QLineEdit>

using namespace GammaRay;

ConnectionInspector::ConnectionInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::ConnectionInspector)
{
  ui->setupUi(this);

  ConnectionFilterProxyModel *proxy = new ConnectionFilterProxyModel(this);
  new ProxyDetacher(ui->connectionView, proxy, probe->connectionModel());
  ui->connectionSearchLine->setProxy(proxy);
  ui->connectionView->setModel(proxy);

  if (qgetenv("GAMMARAY_TEST_FILTER") == "1") {
    QMetaObject::invokeMethod(ui->connectionSearchLine->lineEdit(), "setText",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QLatin1String("destroyed")));
  }
}

#include "connectioninspector.moc"
