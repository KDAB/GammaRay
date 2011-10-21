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

#ifndef GAMMARAY_STATEMACHINEVIEWER_H
#define GAMMARAY_STATEMACHINEVIEWER_H

#include <toolfactory.h>

#include <QHash>
#include <QQueue>
#include <QGraphicsView>
#include <QString>
#include <QWidget>

#include "gvgraph/gvtypes.h"

class QAbstractTransition;
class QStateMachine;
class QAbstractState;
class QGraphicsItem;
class QAbstractItemModel;
class QModelIndex;
class QGraphicsView;

typedef QSet<QAbstractState*> StateMachineConfiguration;

namespace GammaRay {

class StateMachineWatcher;

class GVNodeItem;
class GVEdgeItem;
class GVGraphItem;

namespace Ui { class StateMachineViewer; }

class StateModel;
class TransitionModel;

class GVGraph;

template<class T>
class RingBuffer
{
public:
  RingBuffer() : m_size(5) {}

  void resize(int size)
  {
    Q_ASSERT(size > 0);
    m_size = size;
    cleanup();
  }

  void enqueue(T t)
  {
    m_entries.enqueue(t);
    cleanup();
  }

  void clear() { m_entries.clear(); }
  int size() const { return m_entries.size(); }
  T tail() const { return m_entries.last(); }

  QList<T> entries() const { return m_entries; }

private:
  void cleanup()
  {
    while (m_entries.size() > m_size)
      m_entries.dequeue();
  }

  QQueue<T> m_entries;
  int m_size;
};

class StateMachineView : public QGraphicsView
{
  Q_OBJECT

public:
  explicit StateMachineView(QWidget* parent = 0);
  explicit StateMachineView(QGraphicsScene* scene, QWidget* parent = 0);

public Q_SLOTS:
  void zoomBy(qreal scaleFactor);

protected:
  virtual void wheelEvent(QWheelEvent* event);
};

class StateMachineViewer : public QWidget
{
  Q_OBJECT

public:
  explicit StateMachineViewer(ProbeInterface *probe, QWidget *parent = 0);

  void addState(QAbstractState* state);
  void addTransition(QAbstractTransition* transition);

  QStateMachine* selectedStateMachine() const;

private slots:
  void handleStatesChanged();
  void handleTransitionTriggered(QAbstractTransition*);

  void handleMachineClicked(const QModelIndex&);
  void handleStateClicked(const QModelIndex&);
  void handleDepthChanged(int depth);

  void selectStateMachine(QStateMachine* stateMachine);
  void setFilteredState(QAbstractState* state);
  void setMaximumDepth(int depth);

  void clearView();
  void repopulateView();

  void showMessage(const QString& message);

private:
  void clearGraph();
  void repopulateGraph();

  void updateStateItems();
  void updateTransitionItems();

  bool mayAddState(QAbstractState* state);

  QScopedPointer<Ui::StateMachineViewer> m_ui;

  GVGraph* m_graph;
  QFont m_font;

  QAbstractItemModel* m_stateMachineModel;
  StateModel* m_stateModel;
  TransitionModel* m_transitionModel;

  // filters
  QAbstractState* m_filteredState;
  int m_maximumDepth;
  QStateMachine* m_selectedStateMachine;

  QHash<QAbstractTransition*, EdgeId> m_transitionEdgeIdMap;
  QHash<QAbstractState*, GraphId> m_stateGraphIdMap;
  QHash<QAbstractState*, NodeId> m_stateNodeIdMap;

  QHash<EdgeId, GVEdgeItem*> m_edgeItemMap;
  QHash<GraphId, GVGraphItem*> m_graphItemMap;
  QHash<NodeId, GVNodeItem*> m_nodeItemMap;

  RingBuffer<StateMachineConfiguration> m_lastConfigurations;
  RingBuffer<QAbstractTransition*> m_lastTransitions;

  StateMachineWatcher* m_stateMachineWatcher;
};

class StateMachineViewerFactory :
    public QObject, public StandardToolFactory<QStateMachine, StateMachineViewer>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)

public:
  explicit StateMachineViewerFactory(QObject *parent = 0) : QObject(parent) {}
  inline QString name() const { return tr("State Machine Viewer"); }
};

}

#endif // GAMMARAY_STATEMACHINEVIEWER_H
