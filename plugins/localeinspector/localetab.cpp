/*
  localetab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "localetab.h"
#include "ui_localetab.h"

#include <ui/searchlinecontroller.h>
#include <common/objectbroker.h>

using namespace GammaRay;

LocaleTab::LocaleTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LocaleTab)
    , m_stateManager(this)
{
    QAbstractItemModel *localeModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.LocaleModel"));
    QAbstractItemModel *accessorModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.LocaleAccessorModel"));

    ui->setupUi(this);
    ui->accessorTable->horizontalHeader()->setObjectName("accessorTableHHeader");
    ui->accessorTable->verticalHeader()->setObjectName("accessorTableVHeader");
    ui->localeTable->horizontalHeader()->setObjectName("localeTableHHeader");
    ui->localeTable->verticalHeader()->setObjectName("localeTableVHeader");

    ui->localeTable->setModel(localeModel);
    ui->accessorTable->setModel(accessorModel);
    new SearchLineController(ui->localeSearchLine, localeModel);

    ui->accessorTable->resizeColumnsToContents();
    ui->localeTable->resizeColumnsToContents();
    connect(localeModel, &QAbstractItemModel::modelReset, ui->localeTable, &QTableView::resizeColumnsToContents);
    connect(accessorModel, &QAbstractItemModel::dataChanged, ui->accessorTable,
            &QTableView::resizeColumnsToContents);

    QMetaObject::invokeMethod(this, "initSplitterPosition", Qt::QueuedConnection);
    connect(accessorModel, &QAbstractItemModel::rowsInserted,
            this, &LocaleTab::initSplitterPosition);
}

LocaleTab::~LocaleTab() = default;

void LocaleTab::initSplitterPosition()
{
    const int accessorHeight = ui->accessorTable->model()->rowCount()
            * (ui->accessorTable->rowHeight(0) + 1) // + grid line
        + 2 * ui->accessorTable->frameWidth();
    m_stateManager.setDefaultSizes(ui->mainSplitter,
                                   UISizeVector() << accessorHeight << height() - accessorHeight);
    m_stateManager.restoreState();
}
