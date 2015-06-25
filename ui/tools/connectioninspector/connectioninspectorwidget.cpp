/*
  connectioninspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "connectioninspectorwidget.h"
#include "ui_connectioninspectorwidget.h"

#include <common/objectbroker.h>

#include <QLineEdit>
#include <QSortFilterProxyModel>

using namespace GammaRay;

ConnectionInspectorWidget::ConnectionInspectorWidget(QWidget* parent)
  : QWidget(parent),
    ui(new Ui::ConnectionInspectorWidget)
{
  ui->setupUi(this);

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  // TODO: 1.3 had a proxy detacher here, to improve performance
  proxy->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.ConnectionModel"));
  ui->connectionSearchLine->setProxy(proxy);
  ui->connectionView->setModel(proxy);

  if (qgetenv("GAMMARAY_TEST_FILTER") == "1") {
    QMetaObject::invokeMethod(ui->connectionSearchLine->lineEdit(), "setText",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QLatin1String("destroyed")));
  }
}

ConnectionInspectorWidget::~ConnectionInspectorWidget()
{
}
