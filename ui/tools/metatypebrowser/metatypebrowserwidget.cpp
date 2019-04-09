/*
  metatypebrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    ObjectBroker::registerClientObjectFactoryCallback<MetaTypeBrowserInterface*>(createMetaTypeBrowserClient);

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

    auto iface = ObjectBroker::object<MetaTypeBrowserInterface*>();
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
