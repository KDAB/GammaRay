/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWER_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWER_H

#include "statemachineviewerutil.h"
#include "gvgraph/gvtypes.h"

#include "include/toolfactory.h"

#include <QHash>
#include <QSet>
#include <QString>
#include <QWidget>

class QAbstractTransition;
class QStateMachine;
class QAbstractState;
class QAbstractItemModel;
class QModelIndex;

typedef QSet<QAbstractState*> StateMachineConfiguration;

namespace GammaRay {

namespace Ui {
  class StateMachineViewer;
}

class GVNodeItem;
class GVEdgeItem;
class GVGraphItem;
class StateModel;
class StateMachineWatcher;
class TransitionModel;

class GVGraph;

class StateMachineViewer : public QWidget
{
  Q_OBJECT
  public:
    explicit StateMachineViewer(ProbeInterface *probe, QWidget *parent = 0);

    void addState(QAbstractState *state);
    void addTransition(QAbstractTransition *transition);

    QStateMachine *selectedStateMachine() const;

  private slots:
    void handleStatesChanged();
    void handleTransitionTriggered(QAbstractTransition *);

    void handleMachineClicked(const QModelIndex &);
    void handleStateClicked(const QModelIndex &);
    void handleDepthChanged(int depth);

    void selectStateMachine(QStateMachine *stateMachine);
    void setFilteredState(QAbstractState *state);
    void setMaximumDepth(int depth);

    void clearView();
    void repopulateView();

    void showMessage(const QString &message);

    void updateStartStop();
    void startStopClicked();
    void exportAsImage();
    void exportAsDocumentation();

  private:
    void clearGraph();
    void repopulateGraph();

    void updateStateItems();
    void updateTransitionItems();

    bool mayAddState(QAbstractState *state);

    QScopedPointer<Ui::StateMachineViewer> m_ui;

    GVGraph *m_graph;
    QFont m_font;

    QAbstractItemModel *m_stateMachineModel;
    StateModel *m_stateModel;
    TransitionModel *m_transitionModel;

    // filters
    QAbstractState *m_filteredState;
    int m_maximumDepth;

    QHash<QAbstractTransition *, EdgeId> m_transitionEdgeIdMap;
    QHash<QAbstractState *, GraphId> m_stateGraphIdMap;
    QHash<QAbstractState *, NodeId> m_stateNodeIdMap;

    QHash<EdgeId, GVEdgeItem *> m_edgeItemMap;
    QHash<GraphId, GVGraphItem *> m_graphItemMap;
    QHash<NodeId, GVNodeItem *> m_nodeItemMap;

    RingBuffer<StateMachineConfiguration> m_lastConfigurations;
    RingBuffer<QAbstractTransition*> m_lastTransitions;

    StateMachineWatcher *m_stateMachineWatcher;
};

class StateMachineViewerFactory :
    public QObject, public StandardToolFactory<QStateMachine, StateMachineViewer>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.StateMachineViewer")

  public:
    explicit StateMachineViewerFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    inline QString name() const
    {
      return tr("State Machine Viewer");
    }
};

}

#endif // GAMMARAY_STATEMACHINEVIEWER_H
