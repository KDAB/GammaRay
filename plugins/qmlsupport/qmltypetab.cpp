/*
  qmltypetab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "qmltypetab.h"
#include "ui_qmltypetab.h"

#include <ui/clientpropertymodel.h>
#include <ui/contextmenuextension.h>
#include <ui/propertywidget.h>

#include <common/objectbroker.h>
#include <common/propertymodel.h>

#include <QMenu>

using namespace GammaRay;

QmlTypeTab::QmlTypeTab(PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QmlTypeTab)
{
    ui->setupUi(this);

    ui->typeView->header()->setObjectName("qmlTypeViewHeader");
    ui->typeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    auto clientPropModel = new ClientPropertyModel(this);
    clientPropModel->setSourceModel(ObjectBroker::model(parent->objectBaseName() + QStringLiteral(".qmlTypeModel")));
    ui->typeView->setModel(clientPropModel);

    connect(ui->typeView, &QWidget::customContextMenuRequested, this, &QmlTypeTab::contextMenu);
}

QmlTypeTab::~QmlTypeTab() = default;

void QmlTypeTab::contextMenu(QPoint pos)
{
    // TODO share this with PropertiesTab
    const auto idx = ui->typeView->indexAt(pos);
    if (!idx.isValid())
        return;

    const auto actions = idx.data(PropertyModel::ActionRole).toInt();
    const auto objectId = idx.data(PropertyModel::ObjectIdRole).value<ObjectId>();
    ContextMenuExtension ext(objectId);
    const bool canShow = actions != PropertyModel::NoAction
        || ext.discoverPropertySourceLocation(ContextMenuExtension::GoTo, idx);

    if (!canShow)
        return;

    QMenu contextMenu;
    ext.populateMenu(&contextMenu);
    contextMenu.exec(ui->typeView->viewport()->mapToGlobal(pos));
}
