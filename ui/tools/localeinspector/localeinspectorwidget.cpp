/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "localeinspectorwidget.h"
#include "ui_localeinspectorwidget.h"

#include <ui/searchlinecontroller.h>
#include <common/objectbroker.h>

using namespace GammaRay;

LocaleInspectorWidget::LocaleInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LocaleInspectorWidget)
    , m_stateManager(this)
{
    QAbstractItemModel *localeModel
        = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.LocaleModel"));
    QAbstractItemModel *accessorModel
        = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.LocaleAccessorModel"));

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
    connect(localeModel, SIGNAL(modelReset()), ui->localeTable, SLOT(resizeColumnsToContents()));
    connect(accessorModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), ui->accessorTable,
            SLOT(resizeColumnsToContents()));

    QMetaObject::invokeMethod(this, "initSplitterPosition", Qt::QueuedConnection);
    connect(accessorModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            SLOT(initSplitterPosition()));
}

LocaleInspectorWidget::~LocaleInspectorWidget()
{
}

void LocaleInspectorWidget::initSplitterPosition()
{
    const int accessorHeight = ui->accessorTable->model()->rowCount()
                               * (ui->accessorTable->rowHeight(0) + 1)                                      // + grid line
                               + 2 * ui->accessorTable->frameWidth();
    m_stateManager.setDefaultSizes(ui->mainSplitter,
                                   UISizeVector() << accessorHeight << height() - accessorHeight);
    m_stateManager.restoreState();
}
