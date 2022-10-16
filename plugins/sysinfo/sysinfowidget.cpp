/*
  sysinfowidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "sysinfowidget.h"
#include "ui_sysinfowidget.h"

#include <common/objectbroker.h>

using namespace GammaRay;

SysInfoWidget::SysInfoWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SysInfoWidget)
{
    ui->setupUi(this);

    ui->sysInfoView->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.SysInfoModel")));
    ui->sysInfoView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->libInfoView->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.LibraryInfoModel")));
    ui->libInfoView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->envView->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.EnvironmentModel")));
    ui->envView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

SysInfoWidget::~SysInfoWidget() = default;
