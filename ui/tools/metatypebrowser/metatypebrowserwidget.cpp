/*
  metatypebrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "metatypebrowserwidget.h"
#include "ui_metatypebrowserwidget.h"
#include "metatypesclientmodel.h"
#include "metatypebrowserclient.h"

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>
#include <common/objectid.h>
#include <common/tools/metatypebrowser/metatyperoles.h>

#include <QMenu>

using namespace GammaRay;

static QObject *createMetaTypeBrowserClient(const QString & /*name*/, QObject *parent)
{
    return new MetaTypeBrowserClient(parent);
}

MetaTypeBrowserWidget::MetaTypeBrowserWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MetaTypeBrowserWidget)
    , m_stateManager(this)
{
    ObjectBroker::registerClientObjectFactoryCallback<MetaTypeBrowserInterface *>(createMetaTypeBrowserClient);

    ui->setupUi(this);

    auto mtm = new MetaTypesClientModel(this);
    mtm->setSourceModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.MetaTypeModel")));

    ui->metaTypeView->header()->setObjectName("metaTypeViewHeader");
    ui->metaTypeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->metaTypeView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->metaTypeView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->metaTypeView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    ui->metaTypeView->setDeferredResizeMode(4, QHeaderView::ResizeToContents);
    ui->metaTypeView->setModel(mtm);
    ui->metaTypeView->sortByColumn(1, Qt::AscendingOrder); // sort by type id
    connect(ui->metaTypeView, &QWidget::customContextMenuRequested, this, &MetaTypeBrowserWidget::contextMenu);

    new SearchLineController(ui->metaTypeSearchLine, mtm->sourceModel());

    auto iface = ObjectBroker::object<MetaTypeBrowserInterface *>();
    connect(ui->actionRescanTypes, &QAction::triggered, iface, &MetaTypeBrowserInterface::rescanTypes);
    iface->rescanTypes();

    addAction(ui->actionRescanTypes);
}

MetaTypeBrowserWidget::~MetaTypeBrowserWidget() = default;

void MetaTypeBrowserWidget::contextMenu(QPoint pos)
{
    auto index = ui->metaTypeView->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);

    const auto objectId = index.data(MetaTypeRoles::MetaObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(ui->metaTypeView->viewport()->mapToGlobal(pos));
}
