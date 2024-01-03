/*
  networkconfigurationwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "networkconfigurationwidget.h"
#include "ui_networkconfigurationwidget.h"
#include "clientnetworkconfigurationmodel.h"

#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>

using namespace GammaRay;

NetworkConfigurationWidget::NetworkConfigurationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NetworkConfigurationWidget)
{
    ui->setupUi(this);
    auto srcModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.NetworkConfigurationModel"));
    auto model = new ClientNetworkConfigurationModel(this);
    model->setSourceModel(srcModel);
    ui->networkConfigView->setModel(model);
    ui->networkConfigView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    new SearchLineController(ui->searchLine, model);
}

NetworkConfigurationWidget::~NetworkConfigurationWidget() = default;
