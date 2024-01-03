/*
  cookietab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "cookietab.h"
#include "ui_cookietab.h"

#include <ui/propertywidget.h>

#include <common/objectbroker.h>

using namespace GammaRay;

CookieTab::CookieTab(GammaRay::PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CookieTab)
{
    ui->setupUi(this);

    ui->cookieJarView->setModel(ObjectBroker::model(parent->objectBaseName()
                                                    + QStringLiteral(".cookieJarModel")));
}

CookieTab::~CookieTab() = default;
