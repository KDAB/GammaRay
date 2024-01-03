/*
  actioninspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "actioninspectorwidget.h"
#include "ui_actioninspectorwidget.h"
#include "actionmodel.h" // for column enum only
#include "clientactionmodel.h"

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>
#include <common/objectid.h>
#include <common/endpoint.h>

#include <QDebug>
#include <QMenu>

using namespace GammaRay;

ActionInspectorWidget::ActionInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ActionInspectorWidget)
    , m_stateManager(this)
{
    setObjectName("ActionInspectorWidget");
    ui->setupUi(this);

    auto sourceModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));
    auto actionModel = new ClientActionModel(this);
    actionModel->setSourceModel(sourceModel);

    new SearchLineController(ui->actionSearchLine, actionModel);

    ui->actionView->header()->setObjectName("objectTreeViewHeader");
    ui->actionView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->actionView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->actionView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    ui->actionView->setDeferredResizeMode(4, QHeaderView::ResizeToContents);
    ui->actionView->setModel(actionModel);
    ui->actionView->sortByColumn(ActionModel::ShortcutsPropColumn, Qt::AscendingOrder);
    connect(ui->actionView, &QWidget::customContextMenuRequested, this, &ActionInspectorWidget::contextMenu);

    auto selectionModel = ObjectBroker::selectionModel(actionModel);
    ui->actionView->setSelectionModel(selectionModel);
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &ActionInspectorWidget::selectionChanged);

    m_stateManager.setDefaultSizes(ui->actionView->header(), UISizeVector() << -1 << 200 << -1 << -1 << -1 << 200);
    connect(ui->actionView, &QAbstractItemView::doubleClicked, this, &ActionInspectorWidget::triggerAction);
}

ActionInspectorWidget::~ActionInspectorWidget() = default;

void ActionInspectorWidget::triggerAction(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    Endpoint::instance()->invokeObject(QStringLiteral(
                                           "com.kdab.GammaRay.ActionInspector"),
                                       "triggerAction",
                                       QVariantList() << index.row());
}

void ActionInspectorWidget::contextMenu(QPoint pos)
{
    auto index = ui->actionView->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);

    const auto objectId = index.data(ActionModel::ObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(ui->actionView->viewport()->mapToGlobal(pos));
}

void ActionInspectorWidget::selectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    const auto idx = selection.at(0).topLeft();
    ui->actionView->scrollTo(idx);
}
