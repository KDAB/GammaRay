/*
  timezonetab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
