/*
  objectinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "objectinspectorwidget.h"
#include "ui_objectinspectorwidget.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/sourcelocation.h>

#include <ui/clientdecorationidentityproxymodel.h>
#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <QLineEdit>
#include <QMenu>
#include <QItemSelectionModel>
#include <QTimer>

using namespace GammaRay;

ObjectInspectorWidget::ObjectInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ObjectInspectorWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);
    ui->objectPropertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.ObjectInspector"));

    auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ObjectInspectorTree"));
    auto *clientModel = new ClientDecorationIdentityProxyModel(this);
    clientModel->setSourceModel(model);
    ui->objectTreeView->header()->setObjectName("objectTreeViewHeader");
    ui->objectTreeView->setModel(clientModel);
    ui->objectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->objectTreeView->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->objectTreeView->setDeferredResizeMode(1, QHeaderView::Interactive);
    new SearchLineController(ui->objectSearchLine, clientModel, ui->objectTreeView);

    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(ui->objectTreeView->model());
    ui->objectTreeView->setSelectionModel(selectionModel);
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &ObjectInspectorWidget::objectSelectionChanged);

    if (qgetenv("GAMMARAY_TEST_FILTER") == "1") {
        QMetaObject::invokeMethod(ui->objectSearchLine, "setText",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("Object")));
    }

    connect(ui->objectTreeView, &QWidget::customContextMenuRequested,
            this, &ObjectInspectorWidget::objectContextMenuRequested);

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "60%"
                                                                    << "40%");

    connect(ui->objectPropertyWidget, &PropertyWidget::tabsUpdated, this, &ObjectInspectorWidget::propertyWidgetTabsChanged);

    ui->favoritesTreeView->setSourceView(ui->objectTreeView);
    ui->favoritesTreeView->header()->setObjectName(QStringLiteral("favoriteObjectsHeaderView"));
}

ObjectInspectorWidget::~ObjectInspectorWidget() = default;

void ObjectInspectorWidget::objectSelectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    const QModelIndex index = selection.first().topLeft();
    ui->objectTreeView->scrollTo(index);
}

void ObjectInspectorWidget::objectContextMenuRequested(const QPoint &pos)
{
    const auto index = ui->objectTreeView->indexAt(pos);
    if (!index.isValid())
        return;

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    QMenu menu(tr("Object @ %1").arg(QLatin1String("0x") + QString::number(objectId.id(), 16)));
    ContextMenuExtension ext(objectId);
    ext.setLocation(ContextMenuExtension::Creation, index.data(ObjectModel::CreationLocationRole).value<SourceLocation>());
    ext.setLocation(ContextMenuExtension::Declaration,
                    index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.setCanFavoriteItems(true);
    ext.populateMenu(&menu);

    menu.exec(ui->objectTreeView->viewport()->mapToGlobal(pos));
}

void ObjectInspectorWidget::propertyWidgetTabsChanged()
{
    m_stateManager.saveState();
    m_stateManager.reset();
}
