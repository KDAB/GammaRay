/*
  webinspectorwidget.cpp

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

#include "webinspectorwidget.h"
#include "webviewmodel.h"
#include "ui_webinspectorwidget.h"

#include <common/objectmodel.h>
#include <common/objectbroker.h>
#include <common/endpoint.h>

using namespace GammaRay;

WebInspectorWidget::WebInspectorWidget(QWidget* parent)
  : QWidget(parent), ui(new Ui::WebInspectorWidget)
{
  ui->setupUi(this);
  ui->webPageComboBox->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WebPages")));
  connect(ui->webPageComboBox, SIGNAL(activated(int)), SLOT(webPageSelected(int)));
}

WebInspectorWidget::~WebInspectorWidget()
{
}

void WebInspectorWidget::webPageSelected(int index)
{
  QObject *obj = ui->webPageComboBox->itemData(index, ObjectModel::ObjectRole).value<QObject*>();

  // Wk 1, local
  if (QWebPage *page = qobject_cast<QWebPage*>(obj)) {
    page->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    ui->webInspector->setPage(page);
    // webinspector needs a show event to actually show anything, just setting the page is not enough...
    ui->webInspector->hide();
    ui->webInspector->show();

    ui->stack->setCurrentWidget(ui->wk1LocalPage);
  }

  else if (ui->webPageComboBox->itemData(index, WebViewModel::WebKitVersionRole).toInt() == 2) {
    const QUrl serverUrl = Endpoint::instance()->serverAddress();
    if (serverUrl.scheme() == QLatin1String("tcp")) {
      QUrl inspectorUrl;
      inspectorUrl.setScheme(QStringLiteral("http"));
      inspectorUrl.setHost(serverUrl.host());
      inspectorUrl.setPort(Endpoint::defaultPort() + 1);
      ui->webView->setUrl(inspectorUrl);
      ui->stack->setCurrentWidget(ui->wk2Page);
    }
  }

  // WK1, remote
  else {
    ui->stack->setCurrentWidget(ui->wk1RemotePage);
  }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(WebInspectorUiFactory)
#endif
