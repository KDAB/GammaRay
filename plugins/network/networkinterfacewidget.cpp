/*
  networkinterfacewidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
