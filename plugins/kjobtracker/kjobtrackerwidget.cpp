/*
  kjobtrackerwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "kjobtrackerwidget.h"
#include "ui_kjobtrackerwidget.h"

#include <ui/searchlinecontroller.h>
#include <common/objectbroker.h>

using namespace GammaRay;

#include <QDebug>
#include <QtPlugin>
#include <QSortFilterProxyModel>

using namespace GammaRay;

KJobTrackerWidget::KJobTrackerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::KJobTrackerWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.KJobModel"));
    new SearchLineController(ui->searchLine, model);
    ui->jobView->header()->setObjectName("jobViewHeader");
    ui->jobView->setModel(model);
}

KJobTrackerWidget::~KJobTrackerWidget() = default;
