/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "objectvisualizerwidget.h"
#include "vtkcontainer.h"
#include "vtkpanel.h"
#include "vtkwidget.h"

#include <ui/deferredtreeview.h>
#include <ui/searchlinecontroller.h>
#include <common/objectbroker.h>
#include <common/objectmodel.h>

#include <QCoreApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSplitter>

using namespace GammaRay;

GraphViewerWidget::GraphViewerWidget(QWidget *parent)
    : QWidget(parent)
    , m_stateManager(this)
    , mWidget(new GraphWidget(this))
{
    mModel = ObjectBroker::model("com.kdab.GammaRay.ObjectVisualizerModel");

    QVBoxLayout *vbox = new QVBoxLayout;
    auto objectSearchLine = new QLineEdit(this);
    new SearchLineController(objectSearchLine, mModel);
    vbox->addWidget(objectSearchLine);
    DeferredTreeView *objectTreeView = new DeferredTreeView(this);
    objectTreeView->header()->setObjectName("objectTreeViewHeader");
    objectTreeView->setModel(mModel);
    objectTreeView->setSortingEnabled(true);
    vbox->addWidget(objectTreeView);

    mObjectTreeView = objectTreeView;

    QWidget *treeViewWidget = new QWidget(this);
    treeViewWidget->setLayout(vbox);

    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(treeViewWidget);
    splitter->addWidget(mWidget);
    QHBoxLayout *hbox = new QHBoxLayout(this);
    hbox->addWidget(splitter);

    mWidget->vtkWidget()->setModel(mModel);
    mWidget->vtkWidget()->setSelectionModel(mObjectTreeView->selectionModel());
}

GraphViewerWidget::~GraphViewerWidget()
{
}
