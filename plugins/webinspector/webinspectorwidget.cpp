/*
  webinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "webinspectorwidget.h"
#include "ui_webinspectorwidget.h"
#include "webviewmodelroles.h"

#include <common/objectmodel.h>
#include <common/objectbroker.h>
#include <common/endpoint.h>

using namespace GammaRay;

WebInspectorWidget::WebInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WebInspectorWidget)
{
    ui->setupUi(this);
    ui->webPageComboBox->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WebPages")));
    connect(ui->webPageComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
            this, &WebInspectorWidget::webPageSelected);
    webPageSelected(0);
}

WebInspectorWidget::~WebInspectorWidget() = default;

void WebInspectorWidget::webPageSelected(int index)
{
    Q_UNUSED(index);
    const QUrl serverUrl = Endpoint::instance()->serverAddress();
    if (serverUrl.scheme() == QLatin1String("tcp")) {
        QUrl inspectorUrl;
        inspectorUrl.setScheme(QStringLiteral("http"));
        inspectorUrl.setHost(serverUrl.host());
        inspectorUrl.setPort(Endpoint::defaultPort() + 1);
        ui->webView->setUrl(inspectorUrl);
    }
}
