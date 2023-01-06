/*
  qmlcontexttab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "qmlcontexttab.h"
#include "ui_qmlcontexttab.h"

#include <ui/clientpropertymodel.h>
#include <ui/contextmenuextension.h>
#include <ui/propertyeditor/propertyeditordelegate.h>
#include <ui/uiintegration.h>

#include <common/objectbroker.h>
#include <common/propertymodel.h>
#include <common/probecontrollerinterface.h>

#include <QMenu>
#include <QSortFilterProxyModel>

using namespace GammaRay;

QmlContextTab::QmlContextTab(PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QmlContextTab)
{
    ui->setupUi(this);

    auto contextModel = ObjectBroker::model(parent->objectBaseName() + QStringLiteral(".qmlContextModel"));
    ui->contextView->header()->setObjectName("contextViewHeader");
    ui->contextView->setModel(contextModel);
    ui->contextView->setSelectionModel(ObjectBroker::selectionModel(contextModel));
    ui->contextView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    connect(ui->contextView, &QWidget::customContextMenuRequested, this,
            &QmlContextTab::contextContextMenu);

    auto propertyModel = ObjectBroker::model(parent->objectBaseName()
                                             + QStringLiteral(".qmlContextPropertyModel"));
    auto propertyClient = new ClientPropertyModel(this);
    propertyClient->setSourceModel(propertyModel);
    auto propertyProxy = new QSortFilterProxyModel(this);
    propertyProxy->setSourceModel(propertyClient);
    propertyProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    ui->contextPropertyView->header()->setObjectName("contextPropertyViewHeader");
    ui->contextPropertyView->setModel(propertyProxy);
    ui->contextPropertyView->sortByColumn(0, Qt::AscendingOrder);
    ui->contextPropertyView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->contextPropertyView->setItemDelegate(new PropertyEditorDelegate(this));
    connect(ui->contextPropertyView, &QWidget::customContextMenuRequested, this,
            &QmlContextTab::propertiesContextMenu);
}

QmlContextTab::~QmlContextTab() = default;

void QmlContextTab::contextContextMenu(QPoint pos)
{
    auto idx = ui->contextView->indexAt(pos);
    ContextMenuExtension cme;

    if (!cme.discoverSourceLocation(ContextMenuExtension::GoTo,
                                    idx.sibling(idx.row(), 1).data().toUrl()))
        return;

    QMenu contextMenu;
    cme.populateMenu(&contextMenu);
    contextMenu.exec(ui->contextView->viewport()->mapToGlobal(pos));
}

void QmlContextTab::propertiesContextMenu(QPoint pos)
{
    const auto idx = ui->contextPropertyView->indexAt(pos);
    if (!idx.isValid())
        return;

    const auto actions = idx.data(PropertyModel::ActionRole).toInt();
    const auto objectId = idx.data(PropertyModel::ObjectIdRole).value<ObjectId>();
    ContextMenuExtension ext(objectId);
    const bool canShow = (actions == PropertyModel::NavigateTo && !objectId.isNull())
        || ext.discoverPropertySourceLocation(ContextMenuExtension::GoTo, idx);

    if (!canShow)
        return;

    QMenu contextMenu;
    ext.populateMenu(&contextMenu);
    contextMenu.exec(ui->contextPropertyView->viewport()->mapToGlobal(pos));
}
