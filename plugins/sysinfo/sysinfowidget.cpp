/*
  sysinfowidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
