/*
  timezonetab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "timezonetab.h"
#include "ui_timezonetab.h"
#include "timezoneclientmodel.h"
#include "timezoneoffsetdataclientmodel.h"

#include <ui/searchlinecontroller.h>

#include <common/endpoint.h>
#include <common/objectbroker.h>

using namespace GammaRay;

TimezoneTab::TimezoneTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimezoneTab)
{
    ui->setupUi(this);

    if (Endpoint::instance()->objectAddress(QLatin1String("com.kdab.GammaRay.TimezoneModel")) == Protocol::InvalidObjectAddress)
        return;

    auto tzModel = ObjectBroker::model(QLatin1String("com.kdab.GammaRay.TimezoneModel"));
    auto tzProxy = new TimezoneClientModel(this);
    tzProxy->setSourceModel(tzModel);
    ui->tzView->setModel(tzProxy);
    new SearchLineController(ui->tzSearchLine, tzModel);

    auto selModel = ObjectBroker::selectionModel(tzProxy);
    ui->tzView->setSelectionModel(selModel);

    auto offsetModel = ObjectBroker::model(QLatin1String("com.kdab.GammaRay.TimezoneOffsetDataModel"));
    auto offsetProxy = new TimezoneOffsetDataClientModel(this);
    offsetProxy->setSourceModel(offsetModel);
    ui->offsetView->setModel(offsetProxy);
    ui->offsetView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

TimezoneTab::~TimezoneTab() = default;
