/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>
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

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERWIDGET_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERWIDGET_H

#include <ui/tooluifactory.h>
#include "statemachineviewerinterface.h"

#include "statemachineviewerutil.h"

#include "gvgraph/gvtypes.h"

#include <QWidget>
#include <QHash>

class QModelIndex;

namespace GammaRay {

class GVGraph;

namespace Ui {
  class StateMachineViewer;
}

class GVNodeItem;
class GVEdgeItem;
class GVGraphItem;

class StateMachineViewerWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit StateMachineViewerWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~StateMachineViewerWidget();

  private slots:
    void clearView();
    void repopulateView();
    void clearGraph();
    void stateAdded(const GammaRay::StateId state, const GammaRay::StateId parent, const bool hasChildren,
                    const QString &label, const GammaRay::StateType type, const bool connectToInitial);
    void stateConfigurationChanged(const GammaRay::StateMachineConfiguration &config);
    void transitionAdded(const GammaRay::TransitionId transition, const GammaRay::StateId source, const GammaRay::StateId target,
                         const QString &label);
    void transitionTriggered(GammaRay::TransitionId transition, const QString &label);
    void showMessage(const QString &message);
    void statusChanged(const bool haveStateMachine, const bool running);

    void exportAsImage();
    void stateModelReset();
    void setMaximumMegaPixels(int);

  private:
    void updateStateItems();
    void updateTransitionItems();
    int maximumMegaPixels() const;

    QScopedPointer<Ui::StateMachineViewer> m_ui;

    GVGraph *m_graph;
    QFont m_font;

    QHash<TransitionId, EdgeId> m_transitionEdgeIdMap;
    QHash<StateId, GraphId> m_stateGraphIdMap;
    QHash<StateId, NodeId> m_stateNodeIdMap;

    QHash<EdgeId, GVEdgeItem *> m_edgeItemMap;
    QHash<GraphId, GVGraphItem *> m_graphItemMap;
    QHash<NodeId, GVNodeItem *> m_nodeItemMap;
    QHash<NodeId, StateType> m_nodeTypeMap;

    RingBuffer<StateMachineConfiguration> m_lastConfigurations;
    RingBuffer<TransitionId> m_lastTransitions;

    StateMachineViewerInterface *m_interface;
};

class StateMachineViewerUiFactory : public QObject, public StandardToolUiFactory<StateMachineViewerWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolUiFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.StateMachineViewerUi")
};

}

#endif // GAMMARAY_STATEMACHINEVIEWERWIDGET_H
