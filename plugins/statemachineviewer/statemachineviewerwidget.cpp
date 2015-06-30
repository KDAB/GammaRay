/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "statemachineviewerwidget.h"

#include "gvgraph/gvgraph.h"
#include "gvgraph/gvgraphitems.h"

#include "statemachineviewerclient.h"

#include <ui/deferredresizemodesetter.h>
#include <ui/deferredtreeviewconfiguration.h>
#include <ui_statemachineviewer.h>

#include <common/objectbroker.h>

#include <QScrollBar>
#include <QGraphicsItem>
#include <QFileDialog>
#include <QSettings>

#include <iostream>

enum {
  KEY_STATETYPE
};

#include <cmath>

using namespace GammaRay;
using namespace std;

static QObject* createStateMachineViewerClient(const QString &/*name*/, QObject *parent)
{
  return new StateMachineViewerClient(parent);
}

template<class T>
static qreal relativePosition(const QList<T>& list, T t)
{
  const int index = list.indexOf(t);
  Q_ASSERT(index != -1);
  return (index+1.0) / list.size();
}

StateMachineViewerWidget::StateMachineViewerWidget(QWidget *parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , m_ui(new Ui::StateMachineViewer)
  , m_graph(new GVGraph("State Machine"))
  , m_font(QFont("Helvetica [Cronxy]", 6))
  , m_interface(0)
{
  m_lastConfigurations.resize(5);

  ObjectBroker::registerClientObjectFactoryCallback<StateMachineViewerInterface*>(createStateMachineViewerClient);
  m_interface = ObjectBroker::object<StateMachineViewerInterface*>();

  m_ui->setupUi(this);

  m_graph->setFont(m_font);

  m_ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
  m_ui->graphicsView->setScene(new QGraphicsScene(this));
  m_ui->graphicsView->setRenderHint(QPainter::Antialiasing);

  QAbstractItemModel *stateMachineModel = ObjectBroker::model("com.kdab.GammaRay.StateMachineModel");
  m_ui->stateMachinesView->setModel(stateMachineModel);
  m_ui->stateMachinesView->setSelectionModel(ObjectBroker::selectionModel(stateMachineModel));
  new DeferredResizeModeSetter(m_ui->stateMachinesView->header(), 0, QHeaderView::Stretch);
  new DeferredResizeModeSetter(m_ui->stateMachinesView->header(), 1, QHeaderView::ResizeToContents);
  new DeferredTreeViewConfiguration(m_ui->stateMachinesView, false);

  QAbstractItemModel *stateModel = ObjectBroker::model("com.kdab.GammaRay.StateModel");
  connect(stateModel, SIGNAL(modelReset()), this, SLOT(stateModelReset()));

  m_ui->singleStateMachineView->setModel(stateModel);
  m_ui->singleStateMachineView->setSelectionModel(ObjectBroker::selectionModel(stateModel));
  m_ui->singleStateMachineView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  new DeferredResizeModeSetter(m_ui->singleStateMachineView->header(), 0, QHeaderView::Stretch);
  new DeferredResizeModeSetter(m_ui->singleStateMachineView->header(), 1, QHeaderView::ResizeToContents);
  new DeferredTreeViewConfiguration(m_ui->singleStateMachineView, true, false);

  connect(m_ui->depthSpinBox, SIGNAL(valueChanged(int)), m_interface, SLOT(setMaximumDepth(int)));
  connect(m_ui->startStopButton, SIGNAL(clicked()), m_interface, SLOT(toggleRunning()));
  connect(m_ui->exportButton, SIGNAL(clicked()), SLOT(exportAsImage()));

  m_ui->maxMegaPixelsSpinBox->setValue(maximumMegaPixels());
  connect(m_ui->maxMegaPixelsSpinBox, SIGNAL(valueChanged(int)), SLOT(setMaximumMegaPixels(int)));

  connect(m_interface, SIGNAL(maximumDepthChanged(int)), m_ui->depthSpinBox, SLOT(setValue(int)));
  connect(m_interface, SIGNAL(message(QString)), this, SLOT(showMessage(QString)));
  connect(m_interface, SIGNAL(aboutToRepopulateGraph()), this, SLOT(clearGraph()));
  connect(m_interface, SIGNAL(graphRepopulated()), this, SLOT(repopulateView()));
  connect(m_interface, SIGNAL(stateConfigurationChanged(GammaRay::StateMachineConfiguration)),
          this, SLOT(stateConfigurationChanged(GammaRay::StateMachineConfiguration)));
  connect(m_interface, SIGNAL(stateAdded(GammaRay::StateId,GammaRay::StateId,bool,QString,GammaRay::StateType,bool)),
          this, SLOT(stateAdded(GammaRay::StateId,GammaRay::StateId,bool,QString,GammaRay::StateType,bool)));
  connect(m_interface, SIGNAL(transitionAdded(GammaRay::TransitionId,GammaRay::StateId,GammaRay::StateId,QString)),
          this, SLOT(transitionAdded(GammaRay::TransitionId,GammaRay::StateId,GammaRay::StateId,QString)));
  connect(m_interface, SIGNAL(statusChanged(bool,bool)), this, SLOT(statusChanged(bool,bool)));
  connect(m_interface, SIGNAL(transitionTriggered(GammaRay::TransitionId,QString)),
          this, SLOT(transitionTriggered(GammaRay::TransitionId,QString)));

  m_interface->repopulateGraph();
}

StateMachineViewerWidget::~StateMachineViewerWidget()
{
}

void StateMachineViewerWidget::statusChanged(const bool haveStateMachine, const bool running)
{
  if (!running) {
    m_ui->startStopButton->setChecked(false);
    m_ui->startStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  } else {
    m_ui->startStopButton->setChecked(true);
    m_ui->startStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
  }
  m_ui->startStopButton->setEnabled(haveStateMachine);
}

void StateMachineViewerWidget::clearView()
{
  m_ui->graphicsView->scene()->clear();
}

void StateMachineViewerWidget::repopulateView()
{
  clearView();

  m_graph->applyLayout();

  QGraphicsScene *scene = m_ui->graphicsView->scene();
  Q_FOREACH (const GVNodePair &nodePair, m_graph->gvNodes()) {
    const NodeId &id = nodePair.first;
    const GVNode &node = nodePair.second;
    GVNodeItem *item = new GVNodeItem(node);
    item->setData(KEY_STATETYPE, QVariant::fromValue(m_nodeTypeMap.value(id, OtherState)));
    scene->addItem(item);
    m_nodeItemMap.insert(id, item);
  }

  Q_FOREACH (const GVEdgePair &edgePair, m_graph->gvEdges()) {
    const EdgeId &id = edgePair.first;
    const GVEdge &edge = edgePair.second;
    GVEdgeItem *item = new GVEdgeItem(edge);
    scene->addItem(item);
    m_edgeItemMap.insert(id, item);
  }

  Q_FOREACH (const GVSubGraphPair &graphPair, m_graph->gvSubGraphs()) {
    const GraphId &id = graphPair.first;
    const GVSubGraph &graph = graphPair.second;
    GVGraphItem *item = new GVGraphItem(graph);
    scene->addItem(item);
    m_graphItemMap.insert(id, item);
  }

  updateStateItems();
  updateTransitionItems();

  // correctly set the scene rect
  scene->setSceneRect(scene->itemsBoundingRect());
}

void StateMachineViewerWidget::clearGraph()
{
  m_graph->clear();

  m_graphItemMap.clear();
  m_nodeItemMap.clear();
  m_edgeItemMap.clear();

  m_stateGraphIdMap.clear();
  m_stateNodeIdMap.clear();
  m_transitionEdgeIdMap.clear();
  m_nodeTypeMap.clear();

  m_lastTransitions.clear();
  m_lastConfigurations.clear();
}

void StateMachineViewerWidget::transitionTriggered(TransitionId transition, const QString &label)
{
  showMessage(tr("Transition triggered: %1").arg(label));

  m_lastTransitions.enqueue(transition);
  updateTransitionItems();
}

void StateMachineViewerWidget::showMessage(const QString &message)
{
  // update log
  QPlainTextEdit *plainTextEdit = m_ui->plainTextEdit;
  plainTextEdit->appendPlainText(message);

  // auto-scroll hack
  QScrollBar *sb = plainTextEdit->verticalScrollBar();
  sb->setValue(sb->maximum());
}

void StateMachineViewerWidget::updateTransitionItems()
{
  // restore default color
  Q_FOREACH (QGraphicsItem *item, m_ui->graphicsView->scene()->items()) {
    GVEdgeItem *edgeItem = qgraphicsitem_cast<GVEdgeItem*>(item);
    if (edgeItem) {
      edgeItem->setPen(QPen());
    }
  }

  // set color based on recent usage
  Q_FOREACH (TransitionId t, m_lastTransitions.entries()) {
    EdgeId id = m_transitionEdgeIdMap.value(t);
    GVEdgeItem *edgeItem = m_edgeItemMap.value(id);
    if (!edgeItem) {
      continue;
    }

    QColor color(Qt::red);
    color.setRedF(relativePosition(m_lastTransitions.entries(), t));
    QPen pen(Qt::DashLine);
    pen.setWidth(2);
    pen.setColor(color);
    edgeItem->setPen(pen);
  }
}

void StateMachineViewerWidget::stateConfigurationChanged(const StateMachineConfiguration &config)
{
  if (m_lastConfigurations.size() > 0 && m_lastConfigurations.tail() == config) {
    return;
  }

  m_lastConfigurations.enqueue(config);
  updateStateItems();
}

void StateMachineViewerWidget::updateStateItems()
{
  // initialize
  Q_FOREACH (GVNodeItem* item, m_nodeItemMap) {
    Q_ASSERT(item);
    QColor color;
    switch (item->data(KEY_STATETYPE).value<StateType>()) {
      case FinalState:
        color = QColor(Qt::black);
        break;
      case StateMachineState:
        color = QColor(Qt::gray);
        break;
      default:
        color = QColor(Qt::white);
        break;
    }
    QBrush brush = item->brush();
    if (brush.style() == Qt::NoBrush)
      brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);
    item->setBrush(brush);
  }

  // color recent configurations based on last usage
  // note that each configuration has the same color saturation atm
  Q_FOREACH (const StateMachineConfiguration &config, m_lastConfigurations.entries()) {
    const qreal alpha = relativePosition(m_lastConfigurations.entries(), config);
    Q_FOREACH (StateId state, config) {
      NodeId id = m_stateNodeIdMap.value(state);
      GVNodeItem *nodeItem = m_nodeItemMap.value(id);
      if (!nodeItem) {
        continue;
      }

      QColor color(Qt::red);
      color.setAlphaF(alpha);
      QBrush brush = nodeItem->brush();
      brush.setColor(color);
      nodeItem->setBrush(brush);
    }
  }
}

void StateMachineViewerWidget::stateAdded(const StateId state, const StateId parent, const bool hasChildren,
                                          const QString &label, const StateType type, const bool connectToInitial)
{
  if (m_stateNodeIdMap.contains(state)) {
    return;
  }

  const GraphId parentGraphId = m_stateGraphIdMap.value(parent);

  GraphId graphId = parentGraphId;

  const QString stateId = QString::number(state);
  if (parent && parentGraphId) {
    if (hasChildren) {
      // only create sub-graphs if we have child states
      graphId = m_graph->addGraph(stateId, parentGraphId);
      m_graph->setGraphAttr(QLatin1String("label"), label, graphId);
    }
  } else {
    graphId = m_graph->addGraph(stateId);
    m_graph->setGraphAttr(QLatin1String("label"), label, graphId);
  }
  Q_ASSERT(graphId);

  const NodeId nodeId = m_graph->addNode(stateId, graphId);
  Q_ASSERT(nodeId);
  m_graph->setNodeAttribute(nodeId, QLatin1String("label"), label);
  m_nodeTypeMap.insert(nodeId, type);

  switch (type) {
    case FinalState:
      m_graph->setNodeAttribute(nodeId, "GammaRayStateType", "final");
      m_graph->setNodeAttribute(nodeId, "shape", "doublecircle");
      m_graph->setNodeAttribute(nodeId, "label", "");
      m_graph->setNodeAttribute(nodeId, "style", "filled");
      m_graph->setNodeAttribute(nodeId, "fillcolor", "black");
      m_graph->setNodeAttribute(nodeId, "fixedsize", "true");
      m_graph->setNodeAttribute(nodeId, "heigh", "0.15");
      m_graph->setNodeAttribute(nodeId, "width", "0.15");
      break;
    case ShallowHistoryState:
      m_graph->setNodeAttribute(nodeId, "GammaRayStateType", "shallowhistory");
      m_graph->setNodeAttribute(nodeId, "label", "H");
      m_graph->setNodeAttribute(nodeId, "shape", "circle");
      break;
    case DeepHistoryState:
      m_graph->setNodeAttribute(nodeId, "GammaRayStateType", "deephistory");
      m_graph->setNodeAttribute(nodeId, "label", "H*");
      m_graph->setNodeAttribute(nodeId, "shape", "circle");
      break;
    case StateMachineState:
    case OtherState:
      m_graph->setNodeAttribute(nodeId, "GammaRayStateType", "other");
      m_graph->setNodeAttribute(nodeId, "shape", "rectangle");
      m_graph->setNodeAttribute(nodeId, "style", "rounded");
      break;
  }

  if (connectToInitial && parentGraphId) {
    NodeId initialNode = m_graph->addNode(QString("initial-%1").arg(QString::number(parent)), parentGraphId);
    m_graph->addEdge(initialNode, nodeId, QString());
    m_graph->setNodeAttribute(initialNode, "shape", "circle");
    m_graph->setNodeAttribute(initialNode, "style", "filled");
    m_graph->setNodeAttribute(initialNode, "fillcolor", "black");
    m_graph->setNodeAttribute(initialNode, "fixedsize", "true");
    m_graph->setNodeAttribute(initialNode, "heigh", "0.1");
    m_graph->setNodeAttribute(initialNode, "width", "0.1");
    m_graph->setNodeAttribute(initialNode, "label", "");
  }

  m_stateGraphIdMap.insert(state, graphId);
  m_stateNodeIdMap.insert(state, nodeId);
}

void StateMachineViewerWidget::transitionAdded(const TransitionId transition, const StateId source, const StateId target, const QString &label)
{
  NodeId sourceStateId = m_stateNodeIdMap.value(source);
  NodeId targetStateId = m_stateNodeIdMap.value(target);
  if (!sourceStateId || !targetStateId) {
    return;
  }

  EdgeId id = m_graph->addEdge(sourceStateId, targetStateId, QString::number(transition));
  Q_ASSERT(id);

  if (!label.isEmpty()) {
    m_graph->setEdgeAttribute(id, QLatin1String("label"), label);
  }

  m_transitionEdgeIdMap.insert(transition, id);
}

int StateMachineViewerWidget::maximumMegaPixels() const
{
    return QSettings().value("StateMachineViewerServer/maximumMegaPixels", 10).toInt();
}

void StateMachineViewerWidget::setMaximumMegaPixels(int megaPixels)
{
    QSettings().setValue("StateMachineViewerServer/maximumMegaPixels", megaPixels);
}

void StateMachineViewerWidget::exportAsImage()
{
  QSettings settings;
  const QString key = QLatin1String("StateMachineViewerServer/imageDir");
  QString lastDir = settings.value(key).toString();

  const QString fileName = QFileDialog::getSaveFileName(this, tr("Save As Image"),
                                                        lastDir, tr("Images (*.png *.jpg *.jpeg)"));
  if (fileName.isEmpty()) {
    return;
  }

  lastDir = QFileInfo(fileName).absolutePath();
  settings.setValue(key, lastDir);

  QGraphicsView* view = m_ui->graphicsView;
  const QRectF sceneRect = view->transform().mapRect(view->sceneRect());
  QSizeF size(sceneRect.width(), sceneRect.height());

  // limit mega pixels
  const double maxPixels = maximumMegaPixels() * 1E+6;
  const double actualMegaPixels = size.width() * size.height();
  if (actualMegaPixels > maxPixels && actualMegaPixels != 0) {
    size *= sqrt(maxPixels / actualMegaPixels);
  }

  int quality = -1;
  const char* format;
  if (fileName.endsWith(QLatin1String("jpg"), Qt::CaseInsensitive)
        || fileName.endsWith(QLatin1String("jpeg"), Qt::CaseInsensitive)) {
    format = "JPG";
    quality = 90;
  } else {
    format = "PNG";
  }

  QImage image(size.width() , size.height(), QImage::Format_ARGB32_Premultiplied);
  image.fill(QColor(Qt::white).rgb());

  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  view->scene()->render(&painter);

  image.save(fileName, format, quality);
}

void StateMachineViewerWidget::stateModelReset()
{
  m_ui->singleStateMachineView->expandAll();
  m_lastTransitions.clear();
  m_lastConfigurations.clear();
}


#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(StateMachineViewerUiFactory)
#endif
