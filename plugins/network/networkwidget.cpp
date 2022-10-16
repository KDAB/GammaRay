/*
  networkwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
