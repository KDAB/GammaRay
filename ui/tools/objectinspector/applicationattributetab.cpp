/*
  applicationattributetab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "applicationattributetab.h"
#include "ui_applicationattributetab.h"

#include <ui/propertywidget.h>

#include <common/objectbroker.h>

using namespace GammaRay;

ApplicationAttributeTab::ApplicationAttributeTab(PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ApplicationAttributeTab)
{
    ui->setupUi(this);
    ui->attributeView->header()->setObjectName("attributeViewHeader");

    ui->attributeView->setModel(ObjectBroker::model(parent->objectBaseName()
                                                    + QStringLiteral(".applicationAttributeModel")));
}

ApplicationAttributeTab::~ApplicationAttributeTab() = default;
