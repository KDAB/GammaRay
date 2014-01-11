/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "gvgraph/gvgraph.h"
#include "gvgraph/gvgraphitems.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsView>

using namespace GammaRay;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // build data
  GVGraph *graph = new GVGraph("Graph");
  NodeId nid1 = graph->addNode("Node1");

  GraphId gid1 = graph->addGraph("SubGraph1");
  NodeId nid2 = graph->addNode("Node2", gid1);
  NodeId nid3 = graph->addNode("Node3", gid1);

  EdgeId eid1 = graph->addEdge(nid1, nid2, "Edge1");
  graph->setEdgeAttribute(eid1, "label", "Edge1");
  EdgeId eid2 = graph->addEdge(nid2, nid3, "Edge2");
  graph->setEdgeAttribute(eid2, "label", "Edge2");
  EdgeId eid3 = graph->addEdge(nid1, nid3, "Edge3");
  graph->setEdgeAttribute(eid3, "label", "Edge3");
  graph->applyLayout();

  if (argc > 1 && strcmp(argv[1], "--no-gui") == 0) {
    return 0;
  }

  // build ui
  QGraphicsView *view = new QGraphicsView;
  view->setRenderHint(QPainter::Antialiasing);
  QGraphicsScene *scene = new QGraphicsScene;
  view->setScene(scene);

  Q_FOREACH (const GVNodePair &pair, graph->gvNodes()) {
    const GVNode node = pair.second;
    QGraphicsItem *item = new GVNodeItem(node, 0);
    scene->addItem(item);
  }
  Q_FOREACH (const GVEdgePair &pair, graph->gvEdges()) {
    const GVEdge edge = pair.second;
    QGraphicsItem *item = new GVEdgeItem(edge, 0);
    scene->addItem(item);
  }
  Q_FOREACH (const GVSubGraphPair &pair, graph->gvSubGraphs()) {
    const GVSubGraph graph = pair.second;
    QGraphicsItem *item = new GVGraphItem(graph, 0);
    scene->addItem(item);
  }
  delete graph;
  graph = 0;

  view->show();

  return app.exec();
}
