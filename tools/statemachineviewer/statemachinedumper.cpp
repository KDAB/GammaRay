/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tobias Koenig <tobias.koenig@kdab.com>

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

#include "statemachinedumper.h"

#include <QtCore/QAbstractTransition>
#include <QtCore/QSignalTransition>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFinalState>
#include <QtCore/QHistoryState>

using namespace GammaRay;

static QString uniqueIdentifier(QObject *object)
{
  return (QLatin1String("0x") + QString::number(reinterpret_cast<qlonglong>(object), 16));
}

static QString signalTransitionSignature(const QSignalTransition *transition)
{
  return QString::fromLatin1("%1::%2").arg(transition->senderObject()->objectName())
                                      .arg(QLatin1String(transition->signal().mid(1)));
}

static bool signalTransitionsAreEqual(const QSignalTransition *left, const QAbstractTransition *right)
{
  if (!qobject_cast<const QSignalTransition*>(right))
    return false;

  if (signalTransitionSignature(left) != signalTransitionSignature(qobject_cast<const QSignalTransition*>(right)))
    return false;

  return (left->targetState() == right->targetState());
}

static bool descendantOf(const QObject *ascendant, const QObject *object)
{
  if (object->parent() == 0)
    return false;

  if (object->parent() == ascendant)
    return true;

  return descendantOf(ascendant, object->parent());
}


StateMachineDumper::StateMachineDumper(QObject *parent)
  : QObject(parent)
  , m_stateMachine(0)
{
}

void StateMachineDumper::dump(QStateMachine *stateMachine, const QString &baseDirectory)
{
  m_stateMachine = stateMachine;
  m_baseDirectory = baseDirectory;

  collectStates(m_stateMachine, m_allStates);

  dumpState(m_stateMachine);
}

void StateMachineDumper::dumpState(QAbstractState *abstractState) const
{
  QState *state = qobject_cast<QState*>(abstractState);
  if (!state)
    return;

  static int counter = 1;
  const QString counterString = QString::number(counter).rightJustified(3, '0');
  const QString fileName(m_baseDirectory + QDir::separator() + QString::fromLatin1("%1_%2.dot").arg(counterString, state->objectName()));
  counter++;

  exportState(state, fileName);

  if (isLeafParentState(state))
    return;

  const QList<QAbstractState*> children = state->findChildren<QAbstractState*>();
  foreach (QAbstractState *child, children) {
    if (child->parentState() == state && !child->findChildren<QAbstractState*>().isEmpty()) {
      dumpState(child);
    }
  }
}

void StateMachineDumper::addNode(QAbstractState *state, GVGraph *graph, GraphId graphId, QHash<QAbstractState*, NodeId> &nodeMap, bool isParentState) const
{
  const NodeId nodeId = graph->addNode(uniqueIdentifier(state), graphId);
  graph->setNodeAttribute(nodeId, QLatin1String("label"), state->objectName());

  nodeMap.insert(state, nodeId);

  if (qobject_cast<QFinalState*>(state)) {
    graph->setNodeAttribute(nodeId, "shape", "doublecircle");
    graph->setNodeAttribute(nodeId, "label", "");
    graph->setNodeAttribute(nodeId, "style", "filled");
    graph->setNodeAttribute(nodeId, "fillcolor", "black");
    graph->setNodeAttribute(nodeId, "fixedsize", "true");
    graph->setNodeAttribute(nodeId, "heigh", "0.15");
    graph->setNodeAttribute(nodeId, "width", "0.15");
  } else if (qobject_cast<QHistoryState*>(state)) {
    graph->setNodeAttribute(nodeId, "label", "H");
    graph->setNodeAttribute(nodeId, "shape", "circle");
  } else {
    graph->setNodeAttribute(nodeId, "shape", "rectangle");
    if (isParentState) {
      graph->setNodeAttribute(nodeId, "style", "invisible");
    } else {
      if (state->findChildren<QAbstractState*>().isEmpty()) {
        graph->setNodeAttribute(nodeId, "style", "rounded,filled");
        graph->setNodeAttribute(nodeId, "fillcolor", "lightgrey");
      } else {
        graph->setNodeAttribute(nodeId, "style", "rounded");
      }
    }
  }

  if (state->parentState() && (state->parentState()->initialState() == state)) {
    NodeId initialNode = graph->addNode(QString("initial-%1").arg(uniqueIdentifier(state->parentState())), graphId);
    graph->addEdge(initialNode, nodeId, QString());
    graph->setNodeAttribute(initialNode, "shape", "circle");
    graph->setNodeAttribute(initialNode, "style", "filled");
    graph->setNodeAttribute(initialNode, "fillcolor", "black");
    graph->setNodeAttribute(initialNode, "fixedsize", "true");
    graph->setNodeAttribute(initialNode, "heigh", "0.1");
    graph->setNodeAttribute(initialNode, "width", "0.1");
    graph->setNodeAttribute(initialNode, "label", "");
  }
}

static QAbstractState* findSiblingOf(QAbstractState* sibling, QAbstractState *state)
{
  if (state == 0)
    return 0;

  if (state->parentState() == sibling->parentState())
    return state;

  return findSiblingOf(sibling, state->parentState());
}

void StateMachineDumper::addTransition(QAbstractState *sourceState, QAbstractTransition *transition, GVGraph *graph, QHash<QAbstractState*, NodeId> &nodeMap, bool isParentState) const
{
  QAbstractState *targetState = transition->targetState();

  QAbstractState *siblingState = findSiblingOf(sourceState, targetState);
  if (siblingState)
    targetState = siblingState;

  if (!nodeMap.contains(targetState)) {
    const NodeId nodeId = graph->addNode(uniqueIdentifier(targetState), graph->rootGraph());
    graph->setNodeAttribute(nodeId, QLatin1String("label"), targetState->objectName());
    graph->setNodeAttribute(nodeId, "shape", "rectangle");
    graph->setNodeAttribute(nodeId, "style", "dashed");
    nodeMap.insert(targetState, nodeId);
  }

  const NodeId sourceNode = nodeMap.value(sourceState);
  const NodeId targetNode = nodeMap.value(targetState);

  EdgeId id = graph->addEdge(sourceNode, targetNode, uniqueIdentifier(transition));

  QSignalTransition *signalTransition = qobject_cast<QSignalTransition*>(transition);
  if (signalTransition) {
    const QString label = QString::fromLatin1("%1::\\n%2").arg(signalTransition->senderObject()->objectName())
                                                          .arg(QLatin1String(signalTransition->signal().mid(1)));
    graph->setEdgeAttribute(id, QLatin1String("label"), label);
  }
  if (isParentState) {
    const QList<GVSubGraphPair> subGraphs = graph->gvSubGraphs();
    graph->setEdgeAttribute(id, QLatin1String("ltail"), subGraphs.first().second.name());
  }
}

void StateMachineDumper::exportState(QState *state, const QString &fileName) const
{
  QFont font("Helvetica [Cronxy]", 11);

  GVGraph *graph = new GVGraph(state->objectName(), GVGraph::DocumentationMode);
  graph->setFont(font);

  GraphId graphId = graph->addGraph(state->objectName());
  graph->setGraphAttr("label", state->objectName(), graphId);

  QHash<QAbstractState*, NodeId> nodeMap;

  addNode(state, graph, graphId, nodeMap, true);

  const QList<QAbstractState*> children = state->findChildren<QAbstractState*>();
  foreach (QAbstractState *child, children) {
    if (child->parentState() == state)
      addNode(child, graph, graphId, nodeMap, false);
  }

  QSet<QAbstractTransition*> childTransitions;
  foreach (QAbstractState *abstractChild, children) {
    QState *child = qobject_cast<QState*>(abstractChild);
    if (!child)
      continue;

    if (child->parentState() == state) {
      QSet<QAbstractTransition*> outgoingTransitionsSet = outgoingTransitions(child);
      foreach (QAbstractTransition *transition, outgoingTransitionsSet) {
        addTransition(child, transition, graph, nodeMap, false);

        bool found = false;
        foreach (QAbstractTransition *t, childTransitions) {
          if (signalTransitionsAreEqual(qobject_cast<QSignalTransition*>(transition), t)) {
            found = true;
            break;
          }
        }

        if (!found)
          childTransitions.insert(transition);
      }
    }
  }

  QSet<QAbstractTransition*> outgoingTransitionsSet = outgoingTransitions(state);
  foreach (QAbstractTransition *transition, outgoingTransitionsSet) {
    bool found = false;
    foreach (QAbstractTransition *t, childTransitions) {
      if (signalTransitionsAreEqual(qobject_cast<QSignalTransition*>(transition), t)) {
        found = true;
        break;
      }
    }

    if (!found) {
      addTransition(state, transition, graph, nodeMap, true);
      childTransitions.insert(transition);
    }
  }

  QState *parentState = state->parentState();
  while (parentState) {
    QSet<QAbstractTransition*> outgoingTransitionsSet = outgoingTransitions(parentState);
    foreach (QAbstractTransition *transition, outgoingTransitionsSet) {
      bool found = false;
      foreach (QAbstractTransition *t, childTransitions) {
        if (signalTransitionsAreEqual(qobject_cast<QSignalTransition*>(transition), t)) {
          found = true;
          break;
        }
      }

      if (!found) {
        addTransition(state, transition, graph, nodeMap, true);
        childTransitions.insert(transition);
      }
    }

    parentState = parentState->parentState();
  }

  graph->saveAs(fileName);
  delete graph;
}

void StateMachineDumper::collectStates(QAbstractState *parentState, QSet<QAbstractState*> &states)
{
  states.insert(parentState);

  QList<QAbstractState*> children = parentState->findChildren<QAbstractState*>();
  foreach (QAbstractState *child, children) {
    collectStates(child, states);
  }
}

bool StateMachineDumper::isLeafParentState(const QState *state) const
{
  QList<QState*> children = state->findChildren<QState*>();
  if (children.isEmpty())
    return false;

  foreach (const QState *child, children) {
    if (!child->findChildren<QState*>().isEmpty())
      return false;
  }

  return true;
}

QSet<QAbstractTransition*> StateMachineDumper::outgoingTransitions(const QState *state) const
{
  QSet<QAbstractTransition*> result;

  const QList<QAbstractTransition*> myTransitions = state->transitions();
  foreach (QAbstractTransition *transition, myTransitions) {
    if (!descendantOf(state, transition->targetState()) && (state != transition->targetState())) {
      result.insert(transition);
    }
  }

  const QList<QState*> children = state->findChildren<QState*>();
  foreach (const QState *child, children) {
    const QList<QAbstractTransition*> transitions = child->transitions();
    foreach (QAbstractTransition *transition, transitions) {
      if (!descendantOf(state, transition->targetState()) && (state != transition->targetState())) {
        if (qobject_cast<QSignalTransition*>(transition)) {
          bool found = false;
          foreach (QAbstractTransition *t, result) {
            if (signalTransitionsAreEqual(qobject_cast<QSignalTransition*>(transition), t)) {
              found = true;
              break;
            }
          }

          if (!found)
            result.insert(transition);
        } else {
          result.insert(transition);
        }
      }
    }
  }

  return result;
}
