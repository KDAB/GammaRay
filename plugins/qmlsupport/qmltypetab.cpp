/*
  qmltypetab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
