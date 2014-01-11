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

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERSERVER_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERSERVER_H

#include "statemachineviewerutil.h"
#include "statemachineviewerinterface.h"

#include <core/toolfactory.h>

#include <QHash>
#include <QSet>
#include <QString>
#include <QVector>
#include <QStateMachine>

#include <config-gammaray.h>

class QAbstractTransition;
class QStateMachine;
class QAbstractState;
class QAbstractItemModel;
class QModelIndex;

namespace GammaRay {

class StateModel;
class StateMachineWatcher;
class TransitionModel;

class StateMachineViewerServer : public StateMachineViewerInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::StateMachineViewerInterface)
  public:
    explicit StateMachineViewerServer(ProbeInterface *probe, QObject *parent = 0);

    void addState(QAbstractState *state);
    void addTransition(QAbstractTransition *transition);

    QStateMachine *selectedStateMachine() const;

    using StateMachineViewerInterface::stateConfigurationChanged;
  private slots:
    void stateEntered(QAbstractState *state);
    void stateExited(QAbstractState *state);
    void stateConfigurationChanged();
    void handleTransitionTriggered(QAbstractTransition *);

    void handleMachineClicked(const QModelIndex &);
    void stateSelectionChanged();

    void setFilteredStates(const QVector<QAbstractState*> &states);
    void setMaximumDepth(int depth);

    void updateStartStop();
    void toggleRunning();

    void repopulateGraph();

  private:

    void updateStateItems();

    bool mayAddState(QAbstractState *state);

    StateModel *m_stateModel;
    TransitionModel *m_transitionModel;

    // filters
    QVector<QAbstractState*> m_filteredStates;
    int m_maximumDepth;

    StateMachineWatcher *m_stateMachineWatcher;
    QSet<QAbstractState*> m_recursionGuard;
    QSet<QAbstractState*> m_lastStateConfig;
};

class StateMachineViewerFactory :
public QObject, public StandardToolFactory<QStateMachine, StateMachineViewerServer>
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

#endif // GAMMARAY_STATEMACHINEVIEWERSERVER_H
