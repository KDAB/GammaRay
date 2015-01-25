/*
  sceneinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "sceneinspectorwidget.h"

#include "scenemodel.h"
#include "sceneinspectorclient.h"
#include "graphicsview.h"
#include "ui_sceneinspectorwidget.h"

#include <common/objectbroker.h>
#include <common/endpoint.h>
#include <common/objectmodel.h>

#include <kde/krecursivefilterproxymodel.h>

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QScrollBar>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

#include <iostream>

using namespace GammaRay;
using namespace std;

static QObject* createClientSceneInspector(const QString &/*name*/, QObject *parent)
{
  return new SceneInspectorClient(parent);
}

SceneInspectorWidget::SceneInspectorWidget(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::SceneInspectorWidget)
  , m_interface(0)
  , m_scene(new QGraphicsScene(this))
  , m_pixmap(new QGraphicsPixmapItem)
  , m_updateTimer(new QTimer(this))
{
  ObjectBroker::registerClientObjectFactoryCallback<SceneInspectorInterface*>(createClientSceneInspector);
  m_interface = ObjectBroker::object<SceneInspectorInterface*>();

  ui->setupUi(this);
  ui->scenePropertyWidget->setObjectBaseName("com.kdab.GammaRay.SceneInspector");

  ui->sceneComboBox->setModel(ObjectBroker::model("com.kdab.GammaRay.SceneList"));
  connect(ui->sceneComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sceneSelected(int)));

  QSortFilterProxyModel *sceneFilter = new KRecursiveFilterProxyModel(this);
  sceneFilter->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.SceneGraphModel"));
  ui->sceneTreeView->setModel(sceneFilter);
  ui->screneTreeSearchLine->setProxy(sceneFilter);

  QItemSelectionModel *itemSelection = ObjectBroker::selectionModel(sceneFilter);
  ui->sceneTreeView->setSelectionModel(itemSelection);
  connect(itemSelection, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(sceneItemSelected(QItemSelection)));

  ui->graphicsSceneView->setGraphicsScene(m_scene);
  connect(m_interface, SIGNAL(sceneRectChanged(QRectF)),
          this, SLOT(sceneRectChanged(QRectF)));
  connect(m_interface, SIGNAL(sceneChanged()),
          this, SLOT(sceneChanged()));
  connect(m_interface, SIGNAL(sceneRendered(QPixmap)),
          this, SLOT(sceneRendered(QPixmap)));
  connect(m_interface, SIGNAL(itemSelected(QRectF)),
          this, SLOT(itemSelected(QRectF)));

  m_interface->initializeGui();

  m_pixmap->setFlag(QGraphicsItem::ItemIgnoresTransformations);
  m_scene->addItem(m_pixmap);
  connect(ui->graphicsSceneView->view(), SIGNAL(transformChanged()),
          this, SLOT(visibleSceneRectChanged()));
  connect(ui->graphicsSceneView->view()->horizontalScrollBar(), SIGNAL(valueChanged(int)),
          this, SLOT(visibleSceneRectChanged()));
  connect(ui->graphicsSceneView->view()->verticalScrollBar(), SIGNAL(valueChanged(int)),
          this, SLOT(visibleSceneRectChanged()));

  if (Endpoint::instance()->isRemoteClient()) {
    ui->graphicsSceneView->view()->viewport()->installEventFilter(this);
  }

  QItemSelectionModel *selection = ObjectBroker::selectionModel(ui->sceneComboBox->model());
  if (selection->currentIndex().isValid()) {
    sceneSelected(selection->currentIndex().row());
  } else if (ui->sceneComboBox->currentIndex() >= 0) { // no server-side selection yet, but there's data available
    sceneSelected(ui->sceneComboBox->currentIndex());
  }

  // limit fps to prevent bad performance, and to group update requests which is esp. required
  // for scrolling and similar high-frequency update requests
  m_updateTimer->setSingleShot(true);
  m_updateTimer->setInterval(100);
  connect(m_updateTimer, SIGNAL(timeout()), SLOT(requestSceneUpdate()));
}

SceneInspectorWidget::~SceneInspectorWidget()
{
}

bool SceneInspectorWidget::eventFilter(QObject *obj, QEvent *event)
{
  Q_ASSERT(obj == ui->graphicsSceneView->view()->viewport());
  if (event->type() == QEvent::Resize) {
    QMetaObject::invokeMethod(this, "visibleSceneRectChanged", Qt::QueuedConnection);
  } else if (event->type() == QEvent::MouseButtonRelease) {
    QMouseEvent *e = static_cast<QMouseEvent*>(event);
    if (e->button() == Qt::LeftButton &&
        e->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier))
    {
      m_interface->sceneClicked(ui->graphicsSceneView->view()->mapToScene(e->pos()));
    }
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
  if (!m_updateTimer->isActive()) {
    m_updateTimer->start();
  }
}

void SceneInspectorWidget::requestSceneUpdate()
{
  if (!Endpoint::instance()->isRemoteClient()) {
    return;
  }

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
    QObject *obj = ui->sceneComboBox->itemData(index, ObjectModel::ObjectRole).value<QObject*>();
    QGraphicsScene *scene = qobject_cast<QGraphicsScene*>(obj);
    cout << Q_FUNC_INFO << ' ' << scene << ' ' << obj << endl;

    if (scene) {
      ui->graphicsSceneView->setGraphicsScene(scene);
    }
  }
}

void SceneInspectorWidget::sceneItemSelected(const QItemSelection &selection)
{
  if (Endpoint::instance()->isRemoteClient()) {
    return;
  }

  QModelIndex index;
  if (!selection.isEmpty())
    index = selection.first().topLeft();

  if (index.isValid()) {
    QGraphicsItem *item = index.data(SceneModel::SceneItemRole).value<QGraphicsItem*>();
    ui->graphicsSceneView->showGraphicsItem(item);
    ui->sceneTreeView->scrollTo(index); // in case selection does not come from us
  }
}
