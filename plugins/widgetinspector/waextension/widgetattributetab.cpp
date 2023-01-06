/*
  widgetattributetab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "widgetattributetab.h"
#include "ui_widgetattributetab.h"

#include <ui/propertywidget.h>

#include <common/objectbroker.h>

using namespace GammaRay;

WidgetAttributeTab::WidgetAttributeTab(PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetAttributeTab)
{
    ui->setupUi(this);
    ui->attributeView->header()->setObjectName("attributeViewHeader");

    ui->attributeView->setModel(ObjectBroker::model(parent->objectBaseName()
                                                    + QStringLiteral(".widgetAttributeModel")));
}

WidgetAttributeTab::~WidgetAttributeTab() = default;
