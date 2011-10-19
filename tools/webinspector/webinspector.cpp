/*
  webinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "webinspector.h"
#include "ui_webinspector.h"
#include <objecttypefilterproxymodel.h>
#include <singlecolumnobjectproxymodel.h>
#include <probeinterface.h>
#include <objectmodel.h>

using namespace GammaRay;

WebInspector::WebInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent), ui(new Ui::WebInspector)
{
  ui->setupUi(this);

  ObjectTypeFilterProxyModel<QWebPage> *webPageFilter =
    new ObjectTypeFilterProxyModel<QWebPage>(this);
  webPageFilter->setSourceModel(probe->objectListModel());
  SingleColumnObjectProxyModel *singleColumnProxy = new SingleColumnObjectProxyModel(this);
  singleColumnProxy->setSourceModel(webPageFilter);
  ui->webPageComboBox->setModel(singleColumnProxy);
  connect(ui->webPageComboBox, SIGNAL(activated(int)), SLOT(webPageSelected(int)));
}

void WebInspector::webPageSelected(int index)
{
  QObject *obj =
    ui->webPageComboBox->itemData(index, ObjectModel::ObjectRole).value<QObject*>();
  QWebPage *page = qobject_cast<QWebPage*>(obj);
  if (page) {
    page->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    ui->webInspector->setPage(page);
  }
}

#include "webinspector.moc"
