/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "gvgraph.h"

#include "gvutils.h"

#include <graphviz/gvc.h>
#ifdef WITH_CGRAPH
#  include <graphviz/cgraph.h>
#else
#  include <graphviz/graph.h>
#endif

#include <QColor>
#include <QDebug>
#include <QStringList>

#include <iostream>

using namespace GammaRay;
using namespace GammaRay::GVUtils;
using namespace std;

/*! Dot uses a 72
 * DPI value for converting it's position coordinates from points to pixels
    while we display at 96 DPI on most operating systems. */
const qreal DotDefaultDPI = 72.0;

GVGraph::GVGraph(const QString &name)
  : _context(gvContext()),
    _graph(0),
    _name(name)
{
  createGraph();
}

void GVGraph::createGraph()
{
  Q_ASSERT(_context);

#ifdef WITH_CGRAPH
  _graph = _agopen(_name, Agdirected, &AgDefaultDisc);
#else
  _graph = _agopen(_name, AGDIGRAPHSTRICT); // Strict directed graph, see libgraph doc
#endif
  Q_ASSERT(_graph);
  _graphMap.insert(_graph, GVSubGraph("ROOT"));

  _agset(_graph, "overlap", "prism");
  _agset(_graph, "splines", "true");
  _agset(_graph, "pad", "0,2");
  _agset(_graph, "dpi", "96,0");
  _agset(_graph, "nodesep", "0,4");

  setFont(_font);
}

void GVGraph::closeGraph()
{
  gvFreeLayout(_context, _graph);

  agclose(_graph);
  _graph = 0;
}

void GVGraph::clear()
{
  closeGraph();

  _graphMap.clear();
  _nodeMap.clear();
  _edgeMap.clear();

  createGraph();
}

GVGraph::~GVGraph()
{
  closeGraph();
}

GraphId GVGraph::addGraph(const QString &name)
{
  return addGraph(name, _graph);
}

GraphId GVGraph::addGraph(const QString &name, GraphId subGraphId)
{
  Agraph_t *graph = agGraph(subGraphId);
  return addGraph(name, graph);
}

GraphId GVGraph::addGraph(const QString &name, Agraph_t *graph)
{
  if (!graph) {
    qWarning() << "Subgraph does not exist:" << graph;
    return 0;
  }

  const QString realName = "cluster" + name;

  Agraph_t *subGraph = _agsubg(graph, realName);
  Q_ASSERT(subGraph);
  _graphMap.insert(subGraph, GVSubGraph(realName));
  return (GraphId)subGraph;
}

void GVGraph::removeGraph(GraphId graphId)
{
  Agraph_t *graph = agGraph(graphId);
  if (!graph) {
    return;
  }

  Agnode_t *node = agfstnode(graph);
  while (node) {
    removeNode(id(node));
    node = agnxtnode(_graph, node);
  }

  agdelete(_graph, graph);
  _graphMap.remove(graph);
}

NodeId GVGraph::addNode(const QString &name, GraphId subGraphId)
{
  Agraph_t *graph = agGraph(subGraphId);
  return addNode(name, graph);
}

NodeId GVGraph::addNode(const QString &name, Agraph_t *graph)
{
  if (!graph) {
    qWarning() << Q_FUNC_INFO << "Node not added, graph is NULL:" << name;
    return 0;
  }

  // TODO: Check for duplicates?
  Agnode_t *node = _agnode(graph, name);
  Q_ASSERT(node);
  _agset(node, "label", name);
  _nodeMap.insert(node, GVNode(name));
  return (NodeId)node;
}

NodeId GVGraph::addNode(const QString &name)
{
  return addNode(name, _graph);
}

QList< NodeId > GVGraph::addNodes(const QStringList &names)
{
  QList<NodeId> ids;
  ids.reserve(names.size());
  for (int i=0; i<names.size(); ++i) {
    ids << addNode(names.at(i));
  }
  return ids;
}

void GVGraph::removeNode(NodeId nodeId)
{
  Agnode_t *node = agNode(nodeId);
  if (!node) {
    return;
  }

  Agedge_t *edge = agfstedge(_graph, node);
  while (edge) {
    removeEdge(id(edge));
    edge = agnxtedge(_graph, edge, node);
  }

  agdelete(_graph, node);
  _nodeMap.remove(node);
}

void GVGraph::clearNodes()
{
  Q_FOREACH (Agnode_t *node, _nodeMap.keys()) {
    removeNode(id(node));
  }
  Q_FOREACH (Agraph_t *graph, _graphMap.keys()) {
    removeGraph(id(graph));
  }
  Q_ASSERT(_graphMap.isEmpty());
  Q_ASSERT(_nodeMap.isEmpty());
  Q_ASSERT(_edgeMap.isEmpty());
}

EdgeId GVGraph::addEdge(NodeId sourceId, NodeId targetId, const QString &name)
{
  Agnode_t *source = agNode(sourceId);
  Agnode_t *target = agNode(targetId);
  if (!source || !target) {
    qWarning() << Q_FUNC_INFO << "Source or target node does not exist:" << source << target;
    return 0;
  }

  Agedge_t *edge = _agedge(_graph, source, target, 0, true);
  Q_ASSERT(edge);
  EdgeId edgeId = id(edge);
  Q_ASSERT(edge);
  _edgeMap.insert(edge, GVEdge(name));
  return edgeId;
}

Agedge_t *GVGraph::agEdge(EdgeId edgeId) const
{
  Agedge_t *edge = (Agedge_t *)edgeId;
  if (!_edgeMap.contains(edge)) {
    return 0;
  }
  return edge;
}

Agraph_t *GVGraph::agGraph(GraphId graphId) const
{
  Agraph_t *graph = (Agraph_t *)graphId;
  if (!_graphMap.contains(graph)) {
    return 0;
  }
  return graph;
}

Agnode_t *GVGraph::agNode(NodeId nodeId) const
{
  Agnode_t *node = (Agnode_t *)nodeId;
  if (!_nodeMap.contains(node)) {
    return 0;
  }
  return node;
}

void GVGraph::removeEdge(EdgeId id)
{
  Agedge_t *edge = agEdge(id);
  if (!edge) {
    return;
  }

  agdelete(_graph, edge);
  _edgeMap.remove(edge);
}

void GVGraph::setFont(const QFont &font)
{
  const QString fontSize = QString::number(font.pointSizeF());

  _agset(_graph, "fontname", font.family());
  _agset(_graph, "fontsize", fontSize);

  _agnodeattr(_graph, "fontname", font.family());
  _agnodeattr(_graph, "fontsize", fontSize);

  _agedgeattr(_graph, "fontname", font.family());
  _agedgeattr(_graph, "fontsize", fontSize);

  _font=font;
}

void GVGraph::applyLayout()
{
  Q_ASSERT(_context);
  Q_ASSERT(_graph);

  gvFreeLayout(_context, _graph);
  _gvLayout(_context, _graph, "dot");

#if 0
  gvRenderFilename(_context, _graph, "png", "/tmp/gvgraph.png");
#endif
}

/// TODO: Is this function even needed to be regularly called?
static qreal dpiForGraph(Agraph_t *graph)
{
  bool ok;
  const qreal dpi = _agget(graph, "dpi", "96,0").replace(',', '.').toDouble(&ok);
  Q_ASSERT(ok);
  return dpi;
}

QRectF GVGraph::boundingRectForAgraph(Agraph_t *graph) const
{
  const qreal dpi = dpiForGraph(graph);
  const qreal left = GD_bb(graph).LL.x * (dpi / DotDefaultDPI);
  const qreal top = (GD_bb(_graph).UR.y - GD_bb(graph).LL.y) * (dpi / DotDefaultDPI);
  const qreal right = GD_bb(graph).UR.x * (dpi / DotDefaultDPI);
  const qreal bottom = (GD_bb(_graph).UR.y - GD_bb(graph).UR.y) * (dpi / DotDefaultDPI);
  return QRectF(left, top, right - left, bottom - top);
}

void GVGraph::setGraphAttr(const QString &attr, const QString &value, GraphId graphId)
{
  if (!graphId) {
    graphId = id(_graph);
  }

  Agraph_t *graph = agGraph(graphId);
  if (!graph) {
    return;
  }

  _agset(graph, attr, value);
}

void GVGraph::setNodeAttribute(NodeId id, const QString &attr, const QString &value)
{
  _agset(agNode(id), attr, value);
}

void GVGraph::setEdgeAttribute(EdgeId id, const QString &attr, const QString &value)
{
  _agset(agEdge(id), attr, value);
}

QRectF GVGraph::boundingRect() const
{
  return boundingRectForAgraph(_graph);
}

QList<GVNodePair> GVGraph::gvNodes() const
{
  QList<GVNodePair> list;
  list.reserve(_nodeMap.size());
  const qreal dpi  = dpiForGraph(_graph);

  Q_FOREACH (Agnode_t *node, _nodeMap.keys()) {
    GVNode object = _nodeMap[node];
    object.m_font = _font;

    //Set the name of the node
    object.m_name = agget(node, const_cast<char *>("label"));

    //Fetch the X coordinate, apply the DPI conversion rate (actual DPI / 72, used by dot)
    qreal x = ND_coord(node).x * (dpi / DotDefaultDPI);

    //Translate the Y coordinate from bottom-left to top-left corner
    qreal y = (GD_bb(_graph).UR.y - ND_coord(node).y) * (dpi / DotDefaultDPI);
    object.m_centerPos = QPoint(x, y);

    //Transform the width and height from inches to pixels
    object.m_height = ND_height(node) * dpi;
    object.m_width = ND_width(node) * dpi;

    if (qstricmp(ND_shape(node)->name, "rectangle") == 0) {
      if (qstricmp(agget(node, const_cast<char *>("style")), "rounded") == 0) {
        object.m_shape = GVNode::RoundedRect;
      } else {
        object.m_shape = GVNode::Rect;
      }
    } else if (qstricmp(ND_shape(node)->name, "doublecircle") == 0) {
      object.m_shape = GVNode::DoubleEllipse;
    }

    if (qstricmp(agget(node, const_cast<char *>("style")), "filled") == 0) {
      object.m_fillColor = QColor(agget(node, const_cast<char *>("fillcolor")));
    }

    list.append(GVNodePair(id(node), object));
  }

  return list;
}

QList<GVEdgePair> GVGraph::gvEdges() const
{
  QList<GVEdgePair> list;
  list.reserve(_edgeMap.size());
  const qreal dpi = dpiForGraph(_graph);

  Q_FOREACH (Agedge_t *edge, _edgeMap.keys()) {
    GVEdge object = _edgeMap[edge];
    object.m_font = _font;

    //Fill the source and target node names
    if (ED_tail_label(edge))
      object.m_source = QString::fromUtf8(ED_tail_label(edge)->text);
    if (ED_head_label(edge))
      object.m_target = QString::fromUtf8(ED_head_label(edge)->text);
    if (ED_label(edge)) {
      object.m_label = QString::fromUtf8(ED_label(edge)->text);

      // note that the position attributes in graphviz point to the *center* of this element.
      // we need to subtract half of the width/height to get the top-left position
#if GRAPHVIZ_MAJOR_VERSION >= 2 && GRAPHVIZ_MINOR_VERSION > 20
      const double posx = ED_label(edge)->pos.x;
      const double posy = ED_label(edge)->pos.y;
#else
      const double posx = ED_label(edge)->p.x;
      const double posy = ED_label(edge)->p.y;
#endif
      object.m_labelBoundingRect = QRectF(
        (posx - ED_label(edge)->dimen.x / 2.0) * (dpi / DotDefaultDPI),
        ((GD_bb(_graph).UR.y - posy) - ED_label(edge)->dimen.y / 2.0) * (dpi / DotDefaultDPI),
        ED_label(edge)->dimen.x * (dpi / DotDefaultDPI),
        ED_label(edge)->dimen.y * (dpi / DotDefaultDPI));
    }

    //Calculate the path from the spline (only one spline, as the graph is strict.
    //If it wasn't, we would have to iterate over the first list too)
    //Calculate the path from the spline (only one as the graph is strict)
    if ((ED_spl(edge)->list != 0) && (ED_spl(edge)->list->size%3 == 1)) {
      //If there is a starting point, draw a line from it to the first curve point
      if (ED_spl(edge)->list->sflag) {
        object.m_path.moveTo(ED_spl(edge)->list->sp.x * (dpi / DotDefaultDPI),
                             (GD_bb(_graph).UR.y - ED_spl(edge)->list->sp.y) * (dpi / DotDefaultDPI));
        object.m_path.lineTo(ED_spl(edge)->list->list[0].x * (dpi / DotDefaultDPI),
                             (GD_bb(_graph).UR.y - ED_spl(edge)->list->list[0].y) *
                             (dpi / DotDefaultDPI));
      } else {
        object.m_path.moveTo(ED_spl(edge)->list->list[0].x * (dpi / DotDefaultDPI),
                             (GD_bb(_graph).UR.y - ED_spl(edge)->list->list[0].y) *
                             (dpi / DotDefaultDPI));
      }

      //Loop over the curve points
      for (int i=1; i<ED_spl(edge)->list->size; i+=3) {
        object.m_path.cubicTo(ED_spl(edge)->list->list[i].x * (dpi / DotDefaultDPI),
                              (GD_bb(_graph).UR.y - ED_spl(edge)->list->list[i].y) *
                              (dpi / DotDefaultDPI),
                              ED_spl(edge)->list->list[i+1].x * (dpi / DotDefaultDPI),
                              (GD_bb(_graph).UR.y - ED_spl(edge)->list->list[i+1].y) *
                              (dpi / DotDefaultDPI),
                              ED_spl(edge)->list->list[i+2].x * (dpi / DotDefaultDPI),
                              (GD_bb(_graph).UR.y - ED_spl(edge)->list->list[i+2].y) *
                              (dpi / DotDefaultDPI));
      }

      //If there is an ending point, draw a line to it
      if(ED_spl(edge)->list->eflag) {
        object.m_path.lineTo(ED_spl(edge)->list->ep.x * (dpi / DotDefaultDPI),
                             (GD_bb(_graph).UR.y - ED_spl(edge)->list->ep.y) * (dpi / DotDefaultDPI));
      }
    }

    Q_ASSERT(!object.m_path.isEmpty());
    list.append(GVEdgePair(id(edge), object));
  }

  return list;
}

QList<GVSubGraphPair> GVGraph::gvSubGraphs() const
{
  QList<GVSubGraphPair> list;

  Q_FOREACH (Agraph_t *subGraph, _graphMap.keys()) {
    if (subGraph == _graph) {
      continue;
    }
    const QRectF rect = boundingRectForAgraph(subGraph);

    QPainterPath path;
    path.addRect(rect);

    GVSubGraph object = _graphMap[subGraph];
    object.m_path = path;
    object.m_name = agnameof(subGraph);

    list.append(GVSubGraphPair(id(subGraph), object));
  }

  return list;
}
