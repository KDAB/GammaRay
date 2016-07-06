/*
  quickinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickinspectorwidget.h"
#include "quickinspectorclient.h"
#include "quickclientitemmodel.h"
#include "quickitemtreewatcher.h"
#include "quickitemmodelroles.h"
#include "quickscenepreviewwidget.h"
#include "geometryextension/sggeometryextensionclient.h"
#include "geometryextension/sggeometrytab.h"
#include "materialextension/materialextensionclient.h"
#include "materialextension/materialtab.h"
#include "quickitemdelegate.h"
#include "ui_quickinspectorwidget.h"

#include <common/endpoint.h>
#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/objectid.h>
#include <common/sourcelocation.h>

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <client/remotemodel.h>

#include <QEvent>
#include <QLabel>
#include <QMenu>
#include <qmath.h>
#include <QRectF>
#include <QtCore/qglobal.h>
#include <QPropertyAnimation>
#include <QSettings>
#include <QDebug>

using namespace GammaRay;

static QObject *createQuickInspectorClient(const QString & /*name*/, QObject *parent)
{
    return new QuickInspectorClient(parent);
}

static QObject *createMaterialExtension(const QString &name, QObject *parent)
{
    return new MaterialExtensionClient(name, parent);
}

static QObject *createSGGeometryExtension(const QString &name, QObject *parent)
{
    return new SGGeometryExtensionClient(name, parent);
}

QuickInspectorWidget::QuickInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QuickInspectorWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    ObjectBroker::registerClientObjectFactoryCallback<QuickInspectorInterface *>(
        createQuickInspectorClient);

    m_interface = ObjectBroker::object<QuickInspectorInterface *>();

    ui->windowComboBox->setModel(ObjectBroker::model(QStringLiteral(
                                                         "com.kdab.GammaRay.QuickWindowModel")));
    connect(ui->windowComboBox, SIGNAL(currentIndexChanged(int)),
            m_interface, SLOT(selectWindow(int)));
    if (ui->windowComboBox->currentIndex() >= 0)
        m_interface->selectWindow(ui->windowComboBox->currentIndex());

    auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickItemModel"));
    auto proxy = new QuickClientItemModel(this);
    proxy->setSourceModel(model);
    ui->itemTreeView->header()->setObjectName("quickItemTreeViewHeader");
    ui->itemTreeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->itemTreeView->setModel(proxy);
    ui->itemTreeView->setItemDelegate(new QuickItemDelegate(ui->itemTreeView));
    new SearchLineController(ui->itemTreeSearchLine, model);
    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(proxy);
    ui->itemTreeView->setSelectionModel(selectionModel);
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(itemSelectionChanged(QItemSelection)));
    connect(proxy, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
            this, SLOT(itemModelDataChanged(QModelIndex,QModelIndex,QVector<int>)));

    model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"));
    ui->sgTreeView->header()->setObjectName("sceneGraphTreeViewHeader");
    ui->sgTreeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->sgTreeView->setModel(model);
    new SearchLineController(ui->sgTreeSearchLine, model);
    QItemSelectionModel *sgSelectionModel = ObjectBroker::selectionModel(model);
    ui->sgTreeView->setSelectionModel(sgSelectionModel);
    connect(sgSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(itemSelectionChanged(QItemSelection)));

    new QuickItemTreeWatcher(ui->itemTreeView, ui->sgTreeView, this);

    m_previewWidget = new QuickScenePreviewWidget(m_interface, this);
    m_previewWidget->setPickSourceModel(proxy);

    ui->itemPropertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.QuickItem"));
    ui->sgPropertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.QuickSceneGraph"));

    ui->previewTreeSplitter->addWidget(m_previewWidget);

    connect(m_interface, SIGNAL(features(GammaRay::QuickInspectorInterface::Features)),
            this, SLOT(setFeatures(GammaRay::QuickInspectorInterface::Features)));

    connect(ui->itemTreeView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(itemContextMenu(QPoint)));

    m_interface->checkFeatures();

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%" << "50%");
    m_stateManager.setDefaultSizes(ui->previewTreeSplitter, UISizeVector() << "50%" << "50%");

    connect(ui->itemPropertyWidget, SIGNAL(tabsUpdated()), this, SLOT(propertyWidgetTabsChanged()));
    connect(ui->sgPropertyWidget, SIGNAL(tabsUpdated()), this, SLOT(propertyWidgetTabsChanged()));
}

QuickInspectorWidget::~QuickInspectorWidget()
{
}

void QuickInspectorWidget::saveTargetState(QSettings *settings) const
{
    settings->setValue("tabIndex", ui->tabWidget->currentIndex());
    settings->setValue("remoteViewState", m_previewWidget->saveState());
}

void QuickInspectorWidget::restoreTargetState(QSettings *settings)
{
    ui->tabWidget->setCurrentIndex(settings->value("tabIndex", 0).toInt());
    m_previewWidget->restoreState(settings->value("remoteViewState").toByteArray());
}

void QuickInspectorWidget::setFeatures(QuickInspectorInterface::Features features)
{
    m_previewWidget->setSupportsCustomRenderModes(features);
}

void QuickInspectorWidget::itemSelectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    const QModelIndex &index = selection.first().topLeft();
    ui->itemTreeView->scrollTo(index);
}

void QuickInspectorWidget::itemModelDataChanged(const QModelIndex &topLeft,
                                                const QModelIndex &bottomRight,
                                                const QVector<int> &roles)
{
    if (!roles.contains(QuickItemModelRole::ItemEvent))
        return;

    for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
        const QModelIndex index = ui->itemTreeView->model()->index(i, 0, topLeft.parent());
        RemoteModel::NodeStates state
            = index.data(RemoteModel::LoadingState).value<RemoteModel::NodeStates>();
        if (state & RemoteModel::Empty || ~state & RemoteModel::Outdated)
            continue;

        QVariantAnimation *colorAnimation = new QVariantAnimation(this);
        QPersistentModelIndex persistentIndex(index);
        connect(colorAnimation, &QVariantAnimation::valueChanged,
                ui->itemTreeView->itemDelegate(), [persistentIndex, this](const QVariant &value) {
            qobject_cast<QuickItemDelegate *>(ui->itemTreeView->itemDelegate())->setTextColor(value,
                                                                                              persistentIndex);
        });
        colorAnimation->setStartValue(QColor(129, 0, 129));
        colorAnimation->setEndValue(QColor(129, 0, 129, 0));
        colorAnimation->setDuration(2000);
        colorAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void QuickInspectorUiFactory::initUi()
{
    ObjectBroker::registerClientObjectFactoryCallback<MaterialExtensionInterface *>(
        createMaterialExtension);

    PropertyWidget::registerTab<MaterialTab>(QStringLiteral("material"), tr("Material"));

    ObjectBroker::registerClientObjectFactoryCallback<SGGeometryExtensionInterface *>(
        createSGGeometryExtension);

    PropertyWidget::registerTab<SGGeometryTab>(QStringLiteral("sgGeometry"), tr("Geometry"));
}

void GammaRay::QuickInspectorWidget::itemContextMenu(const QPoint &pos)
{
    const QModelIndex index = ui->itemTreeView->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu contextMenu;

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    ContextMenuExtension ext(objectId);
    ext.setLocation(ContextMenuExtension::Creation, index.data(
                        ObjectModel::CreationLocationRole).value<SourceLocation>());
    ext.setLocation(ContextMenuExtension::Declaration,
                    index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&contextMenu);
    contextMenu.exec(ui->itemTreeView->viewport()->mapToGlobal(pos));
}

void QuickInspectorWidget::propertyWidgetTabsChanged()
{
    m_stateManager.saveState();
    m_stateManager.reset();
}
