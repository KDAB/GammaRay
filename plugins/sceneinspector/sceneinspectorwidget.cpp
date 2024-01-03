/*
  sceneinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "sceneinspectorwidget.h"

#include "scenemodel.h"
#include "sceneinspectorclient.h"
#include "graphicsview.h"
#include "ui_sceneinspectorwidget.h"

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <common/objectmodel.h>
#include <common/objectbroker.h>
#include <common/endpoint.h>

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QScrollBar>
#include <QMenu>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

#include <iostream>

using namespace GammaRay;
using namespace std;

static QObject *createClientSceneInspector(const QString & /*name*/, QObject *parent)
{
    return new SceneInspectorClient(parent);
}

SceneInspectorWidget::SceneInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SceneInspectorWidget)
    , m_stateManager(this)
    , m_interface(nullptr)
    , m_scene(new QGraphicsScene(this))
    , m_pixmap(new QGraphicsPixmapItem)
    , m_updateTimer(new QTimer(this))
{
    ObjectBroker::registerClientObjectFactoryCallback<SceneInspectorInterface *>(
        createClientSceneInspector);
    m_interface = ObjectBroker::object<SceneInspectorInterface *>();

    ui->setupUi(this);
    ui->sceneTreeView->header()->setObjectName("sceneTreeViewHeader");
    ui->scenePropertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.SceneInspector"));

    ui->sceneComboBox->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.SceneList")));
    connect(ui->sceneComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &SceneInspectorWidget::sceneSelected);

    auto sceneModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.SceneGraphModel"));
    ui->sceneTreeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->sceneTreeView->setModel(sceneModel);
    new SearchLineController(ui->sceneTreeSearchLine, sceneModel);

    QItemSelectionModel *itemSelection = ObjectBroker::selectionModel(sceneModel);
    ui->sceneTreeView->setSelectionModel(itemSelection);
    connect(itemSelection, &QItemSelectionModel::selectionChanged,
            this, &SceneInspectorWidget::sceneItemSelected);

    connect(ui->sceneTreeView, &QWidget::customContextMenuRequested, this,
            &SceneInspectorWidget::sceneContextMenu);

    ui->graphicsSceneView->setGraphicsScene(m_scene);
    connect(m_interface, &SceneInspectorInterface::sceneRectChanged,
            this, &SceneInspectorWidget::sceneRectChanged);
    connect(m_interface, &SceneInspectorInterface::sceneChanged,
            this, &SceneInspectorWidget::sceneChanged);
    connect(m_interface, &SceneInspectorInterface::sceneRendered,
            this, &SceneInspectorWidget::sceneRendered);
    connect(m_interface, &SceneInspectorInterface::itemSelected,
            this, &SceneInspectorWidget::itemSelected);

    m_interface->initializeGui();

    m_pixmap->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    m_scene->addItem(m_pixmap);
    connect(ui->graphicsSceneView->view(), &GraphicsView::transformChanged,
            this, &SceneInspectorWidget::visibleSceneRectChanged);
    connect(ui->graphicsSceneView->view()->horizontalScrollBar(), &QAbstractSlider::valueChanged,
            this, &SceneInspectorWidget::visibleSceneRectChanged);
    connect(ui->graphicsSceneView->view()->verticalScrollBar(), &QAbstractSlider::valueChanged,
            this, &SceneInspectorWidget::visibleSceneRectChanged);

    if (Endpoint::instance()->isRemoteClient())
        ui->graphicsSceneView->view()->viewport()->installEventFilter(this);

    QItemSelectionModel *selection = ObjectBroker::selectionModel(ui->sceneComboBox->model());
    if (selection->currentIndex().isValid())
        sceneSelected(selection->currentIndex().row());
    else if (ui->sceneComboBox->currentIndex() >= 0) // no server-side selection yet, but there's data available
        sceneSelected(ui->sceneComboBox->currentIndex());

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%"
                                                                    << "50%");
    m_stateManager.setDefaultSizes(ui->previewSplitter, UISizeVector() << "50%"
                                                                       << "50%");

    connect(ui->scenePropertyWidget, &PropertyWidget::tabsUpdated, this, &SceneInspectorWidget::propertyWidgetTabsChanged);

    // limit fps to prevent bad performance, and to group update requests which is esp. required
    // for scrolling and similar high-frequency update requests
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(100);
    connect(m_updateTimer, &QTimer::timeout, this, &SceneInspectorWidget::requestSceneUpdate);
}

SceneInspectorWidget::~SceneInspectorWidget() = default;

bool SceneInspectorWidget::eventFilter(QObject *obj, QEvent *event)
{
    Q_ASSERT(obj == ui->graphicsSceneView->view()->viewport());
    if (event->type() == QEvent::Resize) {
        QMetaObject::invokeMethod(this, "visibleSceneRectChanged", Qt::QueuedConnection);
    } else if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        if (e->button() == Qt::LeftButton
            && e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
            m_interface->sceneClicked(ui->graphicsSceneView->view()->mapToScene(e->pos()));
    }
    return QObject::eventFilter(obj, event);
}

void SceneInspectorWidget::itemSelected(const QRectF &boundingRect)
{
    ui->graphicsSceneView->view()->fitInView(boundingRect, Qt::KeepAspectRatio);
    ui->graphicsSceneView->view()->scale(0.8, 0.8);
    visibleSceneRectChanged();
}

void SceneInspectorWidget::sceneRectChanged(const QRectF &rect)
{
    m_scene->setSceneRect(rect);
    visibleSceneRectChanged();
}

void SceneInspectorWidget::sceneChanged()
{
    if (!m_updateTimer->isActive())
        m_updateTimer->start();
}

void SceneInspectorWidget::requestSceneUpdate()
{
    if (!Endpoint::instance()->isRemoteClient())
        return;

    if (ui->graphicsSceneView->view()->rect().isEmpty()) {
        // when the splitter is moved to hide the view, don't request updates
        return;
    }

    m_interface->renderScene(ui->graphicsSceneView->view()->viewportTransform(),
                             ui->graphicsSceneView->view()->viewport()->rect().size());
}

void SceneInspectorWidget::sceneRendered(const QPixmap &view)
{
    m_pixmap->setPixmap(view);
}

void SceneInspectorWidget::visibleSceneRectChanged()
{
    m_pixmap->setPos(ui->graphicsSceneView->view()->mapToScene(0, 0));
    sceneChanged();
}

void SceneInspectorWidget::sceneSelected(int index)
{
    const QModelIndex mi = ui->sceneComboBox->model()->index(index, 0);
    ObjectBroker::selectionModel(ui->sceneComboBox->model())->select(mi, QItemSelectionModel::ClearAndSelect);

    if (!Endpoint::instance()->isRemoteClient()) {
        // for in-process mode, use the user scene directly. This is much more performant and we can
        // skip the pixmap conversions and fps limitations thereof.
        QObject *obj = ui->sceneComboBox->itemData(index, ObjectModel::ObjectRole).value<QObject *>();
        QGraphicsScene *scene = qobject_cast<QGraphicsScene *>(obj);
        cout << Q_FUNC_INFO << ' ' << scene << ' ' << obj << endl;

        if (scene)
            ui->graphicsSceneView->setGraphicsScene(scene);
    }
}

void SceneInspectorWidget::sceneItemSelected(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    const auto index = selection.first().topLeft();
    if (!index.isValid())
        return;

    ui->sceneTreeView->scrollTo(index); // in case selection does not come from us

    if (!Endpoint::instance()->isRemoteClient()) {
        QGraphicsItem *item = index.data(SceneModel::SceneItemRole).value<QGraphicsItem *>();
        ui->graphicsSceneView->showGraphicsItem(item);
    }
}

void SceneInspectorWidget::sceneContextMenu(QPoint pos)
{
    const auto index = ui->sceneTreeView->indexAt(pos);
    if (!index.isValid())
        return;

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    QMenu menu(tr("QGraphicsItem @ %1").arg(QLatin1String("0x") + QString::number(objectId.id(), 16)));
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);

    menu.exec(ui->sceneTreeView->viewport()->mapToGlobal(pos));
}

void SceneInspectorWidget::propertyWidgetTabsChanged()
{
    m_stateManager.saveState();
    m_stateManager.reset();
}

void SceneInspectorUiFactory::initUi()
{
}
