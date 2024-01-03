/*
  networkinterfacewidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "networkinterfacewidget.h"
#include "ui_networkinterfacewidget.h"

#include <common/objectbroker.h>

using namespace GammaRay;

NetworkInterfaceWidget::NetworkInterfaceWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NetworkInterfaceWidget)
{
    ui->setupUi(this);

    ui->interfaceView->setModel(ObjectBroker::model(QStringLiteral(
        "com.kdab.GammaRay.NetworkInterfaceModel")));
    ui->interfaceView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

NetworkInterfaceWidget::~NetworkInterfaceWidget() = default;
