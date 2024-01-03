/*
  networkwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "networkwidget.h"
#include "ui_networkwidget.h"
#include "cookies/cookietab.h"

#include <ui/propertywidget.h>

using namespace GammaRay;

NetworkWidget::NetworkWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NetworkWidget)
{
    ui->setupUi(this);
}

NetworkWidget::~NetworkWidget() = default;

void NetworkWidgetFactory::initUi()
{
    PropertyWidget::registerTab<CookieTab>(QStringLiteral("cookieJar"), tr("Cookies"), PropertyWidgetTabPriority::Basic);
}
