/*
  localeinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "localeinspectorwidget.h"
#include "ui_localeinspectorwidget.h"

#include <common/endpoint.h>

using namespace GammaRay;

LocaleInspectorWidget::LocaleInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LocaleInspectorWidget)
{
    ui->setupUi(this);

    ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->timezoneTab),
                                 Endpoint::instance()->objectAddress(QLatin1String("com.kdab.GammaRay.TimezoneModel")) != Protocol::InvalidObjectAddress);
}

LocaleInspectorWidget::~LocaleInspectorWidget() = default;
