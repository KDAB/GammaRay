/*
  widgetinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
#include <config-gammaray.h>

#include "widgetinspectorwidget.h"
#include "widgetinspectorinterface.h"
#include "widgetinspectorclient.h"
#include "widgetclientmodel.h"
#include "ui_widgetinspectorwidget.h"
#include "waextension/widgetattributetab.h"
#include "widgetmodelroles.h"
#include "widgetremoteview.h"

#include "common/objectbroker.h"
#include "common/objectmodel.h"
#include "common/remotemodelroles.h"

#include <ui/contextmenuextension.h>
#include <ui/paintbufferviewer.h>
#include <ui/searchlinecontroller.h>
#include <ui/uiresources.h>

#include <QActionGroup>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QtPlugin>
#include <QToolBar>
#include <QSettings>
#include <QLayout>
#include <QTabBar>
#include <QTimer>

using namespace GammaRay;

static QObject *createWidgetInspectorClient(const QString & /*name*/, QObject *parent)
{
    return new WidgetInspectorClient(parent);
}

WidgetInspectorWidget::WidgetInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WidgetInspectorWidget)
    , m_stateManager(this)
    , m_inspector(nullptr)
    , m_remoteView(new WidgetRemoteView(this))
    , m_3dView(nullptr)
{
    ObjectBroker::registerClientObjectFactoryCallback<WidgetInspectorInterface *>(
        createWidgetInspectorClient);
    m_inspector = ObjectBroker::object<WidgetInspectorInterface *>();

    ui->setupUi(this);
    ui->widgetPropertyWidget->setObjectBaseName(m_inspector->objectName());

    auto widgetTree = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WidgetTree"));
    auto widgetModel = new WidgetClientModel(this);
    widgetModel->setSourceModel(widgetTree);
    ui->widgetTreeView->header()->setObjectName("widgetTreeViewHeader");
    ui->widgetTreeView->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->widgetTreeView->setDeferredResizeMode(1, QHeaderView::Interactive);
    ui->widgetTreeView->setModel(widgetModel);
    ui->widgetTreeView->setSelectionModel(ObjectBroker::selectionModel(widgetModel));
    new SearchLineController(ui->widgetSearchLine, widgetModel, ui->widgetTreeView);
    connect(ui->widgetTreeView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &WidgetInspectorWidget::widgetSelected);
    connect(ui->widgetTreeView, &QWidget::customContextMenuRequested, this,
            &WidgetInspectorWidget::widgetTreeContextMenu);

    ui->favoritesTreeView->setSourceView(ui->widgetTreeView);
    ui->favoritesTreeView->header()->setObjectName(QStringLiteral("favoriteWidgetsHeaderView"));

    m_remoteView->setName(QStringLiteral("com.kdab.GammaRay.WidgetRemoteView"));
    m_remoteView->setPickSourceModel(widgetModel);
    m_remoteView->setFlagRole(WidgetModelRoles::WidgetFlags);
    m_remoteView->setInvisibleMask(WidgetModelRoles::Invisible);

    auto layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    auto toolbar = new QToolBar(this);
    // Our icons are 16x16 and support hidpi, so let force iconSize on every styles
    toolbar->setIconSize(QSize(16, 16));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    layout->setMenuBar(toolbar);
    ui->widgetPreviewContainer->setLayout(layout);
    layout->addWidget(m_remoteView);

    foreach (auto action, m_remoteView->interactionModeActions()->actions())
        toolbar->addAction(action);
    toolbar->addSeparator();

    auto action = new QAction(UIResources::themedIcon(QLatin1String("active-focus.png")), tr("Show Tab Focus Chain"), this);
    action->setCheckable(true);
    connect(action, &QAction::toggled, m_remoteView, &WidgetRemoteView::setTabFocusOverlayEnabled);
    toolbar->addAction(action);
    toolbar->addSeparator();

    toolbar->addAction(m_remoteView->zoomOutAction());
    auto zoom = new QComboBox;
    // macOS and some platforms expect to use *small* controls in such small toolbar
    zoom->setAttribute(Qt::WA_MacSmallSize);
    zoom->setModel(m_remoteView->zoomLevelModel());
    toolbar->addWidget(zoom);
    connect(zoom, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            m_remoteView, &RemoteViewWidget::setZoomLevel);
    connect(m_remoteView, &RemoteViewWidget::zoomLevelChanged, zoom, &QComboBox::setCurrentIndex);
    zoom->setCurrentIndex(m_remoteView->zoomLevelIndex());
    toolbar->addAction(m_remoteView->zoomInAction());

    connect(ui->actionSaveAsImage, &QAction::triggered, this, &WidgetInspectorWidget::saveAsImage);
    connect(ui->actionSaveAsSvg, &QAction::triggered, this, &WidgetInspectorWidget::saveAsSvg);
    connect(ui->actionSaveAsUiFile, &QAction::triggered, this, &WidgetInspectorWidget::saveAsUiFile);
    connect(ui->actionAnalyzePainting, &QAction::triggered, this, &WidgetInspectorWidget::analyzePainting);

    connect(m_inspector, &WidgetInspectorInterface::featuresChanged, this, &WidgetInspectorWidget::updateActions);

    addAction(ui->actionSaveAsImage);
    addAction(ui->actionSaveAsSvg);
    addAction(ui->actionSaveAsUiFile);
    addAction(ui->actionAnalyzePainting);

    updateActions();

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%"
                                                                    << "50%");
    m_stateManager.setDefaultSizes(ui->previewSplitter, UISizeVector() << "50%"
                                                                       << "50%");
    ui->tabWidget->findChild<QTabBar *>()->hide();

    connect(ui->widgetPropertyWidget, &PropertyWidget::tabsUpdated, this, &WidgetInspectorWidget::propertyWidgetTabsChanged);
}

WidgetInspectorWidget::~WidgetInspectorWidget() = default;

void WidgetInspectorWidget::saveTargetState(QSettings *settings) const
{
    settings->setValue("remoteViewState", m_remoteView->saveState());
}

void WidgetInspectorWidget::restoreTargetState(QSettings *settings)
{
    m_remoteView->restoreState(settings->value("remoteViewState").toByteArray());
}

void WidgetInspectorWidget::updateActions()
{
    const auto model = ui->widgetTreeView->selectionModel()->selectedRows();
    const auto selection = !model.isEmpty() && model.first().isValid();

    ui->actionSaveAsImage->setEnabled(selection);
    ui->actionSaveAsSvg->setEnabled(
        selection && m_inspector->features() & WidgetInspectorInterface::SvgExport);
    ui->actionSaveAsUiFile->setEnabled(
        selection && m_inspector->features() & WidgetInspectorInterface::UiExport);
    ui->actionAnalyzePainting->setEnabled(
        selection && m_inspector->features() & WidgetInspectorInterface::AnalyzePainting);

    auto f = m_remoteView->supportedInteractionModes() & ~RemoteViewWidget::InputRedirection;
    if (m_inspector->features() & WidgetInspectorInterface::InputRedirection)
        f |= RemoteViewWidget::InputRedirection;
    m_remoteView->setSupportedInteractionModes(f);
}

void WidgetInspectorWidget::propertyWidgetTabsChanged()
{
    m_stateManager.saveState();
    m_stateManager.reset();
}

void WidgetInspectorWidget::widgetSelected(const QItemSelection &selection)
{
    ui->tabWidget->setCurrentIndex(0); // select the tree view tab
    QModelIndex index;
    if (!selection.isEmpty())
        index = selection.first().topLeft();

    if (index.isValid()) {
        // in case selection was triggered remotely
        ui->widgetTreeView->scrollTo(index);
    }

    updateActions();
}

void WidgetInspectorWidget::widgetTreeContextMenu(QPoint pos)
{
    const auto index = ui->widgetTreeView->indexAt(pos);
    if (!index.isValid())
        return;

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    QMenu menu(tr("Widget @ %1").arg(QLatin1String("0x") + QString::number(objectId.id(), 16)));
    ContextMenuExtension ext(objectId);
    ext.setCanFavoriteItems(true);
    ext.populateMenu(&menu);

    menu.exec(ui->widgetTreeView->viewport()->mapToGlobal(pos));
}

void WidgetInspectorWidget::saveAsImage()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save As Image"),
        QString(),
        tr("Image Files (*.png *.jpg)"));

    if (fileName.isEmpty())
        return;

    m_inspector->saveAsImage(fileName);
}

void WidgetInspectorWidget::saveAsSvg()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save As SVG"),
        QString(),
        tr("Scalable Vector Graphics (*.svg)"));

    if (fileName.isEmpty())
        return;

    m_inspector->saveAsSvg(fileName);
}

void WidgetInspectorWidget::saveAsUiFile()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save As Qt Designer UI File"),
        QString(),
        tr("Qt Designer UI File (*.ui)"));

    if (fileName.isEmpty())
        return;

    m_inspector->saveAsUiFile(fileName);
}

void WidgetInspectorWidget::analyzePainting()
{
    m_inspector->analyzePainting();

    PaintBufferViewer *viewer = new PaintBufferViewer(QStringLiteral("com.kdab.GammaRay.WidgetPaintAnalyzer"), this);
    viewer->show();
}

void WidgetInspectorUiFactory::initUi()
{
    PropertyWidget::registerTab<WidgetAttributeTab>(QStringLiteral("widgetAttributes"),
                                                    tr(
                                                        "Attributes"),
                                                    PropertyWidgetTabPriority::Advanced);
}
