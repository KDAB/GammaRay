/*
  objectinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectinspectorwidget.h"
#include "ui_objectinspectorwidget.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/sourcelocation.h>

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
    ui->objectTreeView->header()->setObjectName("objectTreeViewHeader");
    ui->objectTreeView->setModel(model);
    ui->objectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->objectTreeView->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->objectTreeView->setDeferredResizeMode(1, QHeaderView::Interactive);
    new SearchLineController(ui->objectSearchLine, model);

    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(ui->objectTreeView->model());
    ui->objectTreeView->setSelectionModel(selectionModel);
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(objectSelectionChanged(QItemSelection)));

    if (qgetenv("GAMMARAY_TEST_FILTER") == "1") {
        QMetaObject::invokeMethod(ui->objectSearchLine, "setText",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, QStringLiteral("Object")));
    }

    connect(ui->objectTreeView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(objectContextMenuRequested(QPoint)));

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "60%" << "40%");

    connect(ui->objectPropertyWidget, SIGNAL(tabsUpdated()), this, SLOT(propertyWidgetTabsChanged()));
}

ObjectInspectorWidget::~ObjectInspectorWidget()
{
}

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
    ext.setLocation(ContextMenuExtension::Creation, index.data(
                        ObjectModel::CreationLocationRole).value<SourceLocation>());
    ext.setLocation(ContextMenuExtension::Declaration,
                    index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&menu);

    menu.exec(ui->objectTreeView->viewport()->mapToGlobal(pos));
}

void ObjectInspectorWidget::propertyWidgetTabsChanged()
{
    m_stateManager.saveState();
    m_stateManager.reset();
}
