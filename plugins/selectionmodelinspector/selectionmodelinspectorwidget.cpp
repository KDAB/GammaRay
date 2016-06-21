/*
  selectionmodelinspectorwidget.cpp

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

#include "selectionmodelinspectorwidget.h"
#include "ui_selectionmodelinspectorwidget.h"

#include <ui/itemdelegate.h>

#include <common/objectmodel.h>
#include <common/objectbroker.h>

using namespace GammaRay;

SelectionModelInspectorWidget::SelectionModelInspectorWidget(QWidget *widget)
    : QWidget(widget)
    , ui(new Ui::SelectionModelInspectorWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    ui->selectionModelView->header()->setObjectName("selectionModelViewHeader");
    ui->selectionModelView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->selectionModelView->setModel(ObjectBroker::model(QStringLiteral(
                                                             "com.kdab.GammaRay.SelectionModelsModel")));
    ui->selectionModelView->setSelectionModel(ObjectBroker::selectionModel(ui->selectionModelView->
                                                                           model()));

    ui->selectionModelVisualizer->header()->setObjectName("selectionModelVisualizerHeader");
    ui->selectionModelVisualizer->setItemDelegate(new ItemDelegate(this));
    ui->selectionModelVisualizer->setModel(ObjectBroker::model(QStringLiteral(
                                                                   "com.kdab.GammaRay.CurrentSelectionModel")));

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "65%" << "35%");
}

SelectionModelInspectorWidget::~SelectionModelInspectorWidget()
{
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(SelectionModelInspectorUiFactory)
#endif
