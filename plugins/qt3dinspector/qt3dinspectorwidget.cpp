/*
  qt3dinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "qt3dinspectorwidget.h"
#include "ui_qt3dinspectorwidget.h"
#include "qt3dinspectorclient.h"
#include "geometryextension/qt3dgeometrytab.h"
#include "geometryextension/qt3dgeometryextensionclient.h"

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/sourcelocation.h>

#include <QMenu>

using namespace GammaRay;

static QObject *create3DInsepctorClient(const QString &/*name*/, QObject *parent)
{
    return new Qt3DInspectorClient(parent);
}

Qt3DInspectorWidget::Qt3DInspectorWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::Qt3DInspectorWidget)
    , m_stateManager(this)
{
    ObjectBroker::registerClientObjectFactoryCallback<Qt3DInspectorInterface*>(create3DInsepctorClient);
    m_interface = ObjectBroker::object<Qt3DInspectorInterface*>();

    ui->setupUi(this);
    ui->engineComboBox->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.Qt3DInspector.engineModel")));
    connect(ui->engineComboBox, SIGNAL(currentIndexChanged(int)), m_interface, SLOT(selectEngine(int)));

    auto sceneModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.Qt3DInspector.sceneModel"));
    ui->sceneTreeView->header()->setObjectName("sceneTreeViewHeader");
    ui->sceneTreeView->setModel(sceneModel);
    ui->sceneTreeView->setSelectionModel(ObjectBroker::selectionModel(sceneModel));
    new SearchLineController(ui->sceneSearchLine, sceneModel);
    connect(ui->sceneTreeView, &QWidget::customContextMenuRequested, this, &Qt3DInspectorWidget::entityContextMenu);

    ui->scenePropertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.Qt3DInspector.entityPropertyController"));

    auto frameGraphModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.Qt3DInspector.frameGraphModel"));
    ui->frameGraphView->header()->setObjectName("frameGraphViewHeader");
    ui->frameGraphView->setModel(frameGraphModel);
    ui->frameGraphView->setSelectionModel(ObjectBroker::selectionModel(frameGraphModel));
    new SearchLineController(ui->frameGraphSearchLine, frameGraphModel);
    connect(ui->frameGraphView, &QWidget::customContextMenuRequested, this, &Qt3DInspectorWidget::frameGraphContextMenu);

    ui->frameGraphNodePropertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.Qt3DInspector.frameGraphPropertyController"));

    connect(ui->tabWidget, &QTabWidget::currentChanged, ui->stack, &QStackedWidget::setCurrentIndex);
    connect(ui->scenePropertyWidget, SIGNAL(tabsUpdated()), &m_stateManager, SLOT(reset()));
    connect(ui->frameGraphNodePropertyWidget, SIGNAL(tabsUpdated()), &m_stateManager, SLOT(reset()));
}

Qt3DInspectorWidget::~Qt3DInspectorWidget()
{
}

void Qt3DInspectorWidget::entityContextMenu(QPoint pos)
{
    const auto index = ui->sceneTreeView->indexAt(pos);
    if (!index.isValid())
        return;

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    QMenu menu(tr("Entity @ %1").arg(QLatin1String("0x") + QString::number(objectId.id(), 16)));
    ContextMenuExtension ext(objectId);
    ext.setCreationLocation(index.data(ObjectModel::CreationLocationRole).value<SourceLocation>());
    ext.setDeclarationLocation(index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&menu);

    menu.exec(ui->sceneTreeView->viewport()->mapToGlobal(pos));
}

void Qt3DInspectorWidget::frameGraphContextMenu(QPoint pos)
{
    const auto index = ui->frameGraphView->indexAt(pos);
    if (!index.isValid())
        return;

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    QMenu menu(tr("Frame Graph Node @ %1").arg(QLatin1String("0x") + QString::number(objectId.id(), 16)));
    ContextMenuExtension ext(objectId);
    ext.setCreationLocation(index.data(ObjectModel::CreationLocationRole).value<SourceLocation>());
    ext.setDeclarationLocation(index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&menu);

    menu.exec(ui->frameGraphView->viewport()->mapToGlobal(pos));
}


static QObject *createGeometryExtension(const QString &name, QObject *parent)
{
  return new Qt3DGeometryExtensionClient(name, parent);
}

void Qt3DInspectorUiFactory::initUi()
{
    ObjectBroker::registerClientObjectFactoryCallback<Qt3DGeometryExtensionInterface*>(createGeometryExtension);
//     PropertyWidget::registerTab<Qt3DGeometryTab>(QStringLiteral("qt3dGeometry"), tr("Geometry"), PropertyWidgetTabPriority::Advanced);
}
