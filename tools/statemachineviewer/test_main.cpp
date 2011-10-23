/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QApplication>
#include <QGraphicsView>
#include "gvgraph/gvgraphitems.h"

using namespace GammaRay;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // build data
  GVGraph *graph = new GVGraph("Graph");
  NodeId nid1 = graph->addNode("Node1");
  NodeId nid2 = graph->addNode("Node2");
  NodeId nid3 = graph->addNode("Node3");

  graph->addEdge(nid1, nid2, "Edge1");
  graph->addEdge(nid2, nid3, "Edge2");
  graph->applyLayout();

  if (argc > 1 && strcmp(argv[1], "--no-gui") == 0) {
    return 0;
  }

  // build ui
  QGraphicsView *view = new QGraphicsView;
  QGraphicsScene *scene = new QGraphicsScene;
  view->setScene(scene);

  Q_FOREACH (const GVNodePair &pair, graph->gvNodes()) {
    const GVNode node = pair.second;
    new GVNodeItem(node, 0, scene);
  }

  Q_FOREACH (const GVEdgePair &pair, graph->gvEdges()) {
    const GVEdge edge = pair.second;
    new GVEdgeItem(edge, 0, scene);
  }
  delete graph;
  graph = 0;

  view->show();

  return app.exec();
}
