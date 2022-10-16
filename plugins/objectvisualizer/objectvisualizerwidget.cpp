/*
  objectvisualizerwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
