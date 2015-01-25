/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "statemachineviewerserver.h"

#include "statemodel.h"
#include "statemachinewatcher.h"
#include "transitionmodel.h"


#include <core/objecttypefilterproxymodel.h>
#include <core/probeinterface.h>
#include <common/objectbroker.h>

#include <QAbstractTransition>
#include <QFinalState>
#include <QHistoryState>
#include <QSignalTransition>
#include <QStateMachine>
#include <QItemSelectionModel>

#include <QtPlugin>

#include <iostream>

using namespace GammaRay;
using namespace std;

namespace {

QString labelForTransition(QAbstractTransition *transition)
{
  const QString objectName = transition->objectName();
  if (!objectName.isEmpty()) {
    return objectName;
  }

  // Try to get a label for the transition if it is a QSignalTransition.
  QSignalTransition *signalTransition = qobject_cast<QSignalTransition*>(transition);
  if (signalTransition) {
      return QString::fromLatin1("%1::%2").
                  arg(Util::displayString(signalTransition->senderObject())).
                  arg(QString::fromLatin1(signalTransition->signal().mid(1)));
  }

  return Util::displayString(transition);
}

}

StateMachineViewerServer::StateMachineViewerServer(ProbeInterface *probe, QObject *parent)
  : StateMachineViewerInterface(parent),
    m_stateModel(new StateModel(this)),
    m_transitionModel(new TransitionModel(this)),
    m_maximumDepth(0),
    m_stateMachineWatcher(new StateMachineWatcher(this))
{
  probe->registerModel("com.kdab.GammaRay.StateModel", m_stateModel);
  QItemSelectionModel *stateSelectionModel = ObjectBroker::selectionModel(m_stateModel);
  connect(stateSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(stateSelectionChanged()));

  ObjectTypeFilterProxyModel<QStateMachine> *stateMachineFilter =
    new ObjectTypeFilterProxyModel<QStateMachine>(this);
  stateMachineFilter->setSourceModel(probe->objectListModel());
  probe->registerModel("com.kdab.GammaRay.StateMachineModel", stateMachineFilter);
  QItemSelectionModel *stateMachineSelectionModel = ObjectBroker::selectionModel(stateMachineFilter);
  connect(stateMachineSelectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(handleMachineClicked(QModelIndex)));

  connect(m_stateMachineWatcher, SIGNAL(stateEntered(QAbstractState*)),
          SLOT(stateEntered(QAbstractState*)));
  connect(m_stateMachineWatcher, SIGNAL(stateExited(QAbstractState*)),
          SLOT(stateExited(QAbstractState*)));
  connect(m_stateMachineWatcher, SIGNAL(transitionTriggered(QAbstractTransition*)),
          SLOT(handleTransitionTriggered(QAbstractTransition*)));



  setMaximumDepth(3);
  updateStartStop();
}

void StateMachineViewerServer::repopulateGraph()
{
  emit aboutToRepopulateGraph();

  // just to be sure the client has the same setting than we do
  emit maximumDepthChanged(m_maximumDepth);
  updateStartStop();

  if (m_filteredStates.isEmpty()) {
    addState(m_stateModel->stateMachine());
  } else {
    foreach(QAbstractState* state, m_filteredStates) {
      addState(state);
    }
  }
  m_recursionGuard.clear();

  emit graphRepopulated();
}

QStateMachine *StateMachineViewerServer::selectedStateMachine() const
{
  return m_stateModel->stateMachine();
}

static int treeDepth(QAbstractState *ascendant, QAbstractState *obj)
{
  if (!Util::descendantOf(ascendant, obj)) {
    return -1;
  }

  int depth = 0;
  QAbstractState *parent = obj->parentState();
  while (parent) {
    ++depth;
    parent = parent->parentState();
  }
  return depth;
}

bool StateMachineViewerServer::mayAddState(QAbstractState *state)
{
  if (!state) {
    return false;
  }

  if (m_recursionGuard.contains(state)) {
    return false;
  }

  if (!m_filteredStates.isEmpty()) {
    bool isValid = false;
    foreach(QAbstractState* filter, m_filteredStates) {
      if (filter == state || Util::descendantOf(filter, state)) {
        isValid = true;
        break;
      }
    }
    if (!isValid) {
      return false;
    }
  }

  if (m_maximumDepth > 0) {
    int depth = -1;
    if (!m_filteredStates.isEmpty()) {
      foreach(QAbstractState* filter, m_filteredStates) {
        depth = ::treeDepth(filter, state);
        if (depth != -1) {
          break;
        }
      }
    } else {
      depth = ::treeDepth(m_stateModel->stateMachine(), state);
    }
    if (depth > m_maximumDepth) {
      return false;
    }
  }

  return true;
}

void StateMachineViewerServer::setFilteredStates(const QVector<QAbstractState*>& states)
{
  if (m_filteredStates == states) {
    return;
  }

  if (states.isEmpty()) {
    emit message(tr("Clearing filter."));
  } else {
    QStringList stateNames;
    stateNames.reserve(states.size());
    foreach(QAbstractState* state, states) {
      stateNames << Util::displayString(state);
    }
    emit message(tr("Setting filter on: %1").arg(stateNames.join(", ")));
  }

  m_filteredStates = states;
}

void StateMachineViewerServer::setMaximumDepth(int depth)
{
  if (m_maximumDepth == depth) {
    return;
  }

  emit message(tr("Showing states until a depth of %1").arg(depth));
  m_maximumDepth = depth;
  repopulateGraph();

  emit maximumDepthChanged(depth);
}

void StateMachineViewerServer::setSelectedStateMachine(QStateMachine* machine)
{
  QStateMachine* oldMachine = selectedStateMachine();
  if (oldMachine) {
    disconnect(oldMachine, SIGNAL(started()), this, SLOT(updateStartStop()));
    disconnect(oldMachine, SIGNAL(stopped()), this, SLOT(updateStartStop()));
    disconnect(oldMachine, SIGNAL(finished()), this, SLOT(updateStartStop()));
  }

  m_stateModel->setStateMachine(machine);
  stateConfigurationChanged();

  setFilteredStates(QVector<QAbstractState*>());
  m_stateMachineWatcher->setWatchedStateMachine(machine);
  repopulateGraph();

  if (machine) {
    connect(machine, SIGNAL(started()), this, SLOT(updateStartStop()));
    connect(machine, SIGNAL(stopped()), this, SLOT(updateStartStop()));
    connect(machine, SIGNAL(finished()), this, SLOT(updateStartStop()));
  }
  updateStartStop();
}

void StateMachineViewerServer::handleMachineClicked(const QModelIndex &index)
{
  if (!index.isValid()) {
    setSelectedStateMachine(0);
    return;
  }

  QObject *stateMachineObject = index.data(ObjectModel::ObjectRole).value<QObject*>();
  QStateMachine *machine = qobject_cast<QStateMachine*>(stateMachineObject);
  setSelectedStateMachine(machine);
}

void StateMachineViewerServer::stateSelectionChanged()
{
  const QModelIndexList& selection = ObjectBroker::selectionModel(m_stateModel)->selectedRows();
  QVector<QAbstractState*> filter;
  filter.reserve(selection.size());
  foreach(const QModelIndex &index, selection) {
    QObject *stateObject = index.data(ObjectModel::ObjectRole).value<QObject*>();
    Q_ASSERT(stateObject);
    QAbstractState *state = qobject_cast<QAbstractState*>(stateObject);
    Q_ASSERT(state);
    bool addState = true;
    /// only pick the top-level items of the selection
    // NOTE: this might be slow for large selections, if someone wants to come up with a better
    // algorithm, please - go for it!
    foreach(QAbstractState *potentialParent, filter) {
      if (Util::descendantOf(potentialParent, state)) {
        addState = false;
        break;
      }
    }

    if (addState) {
      filter << state;
    }
  }
  setFilteredStates(filter);
}

void StateMachineViewerServer::handleTransitionTriggered(QAbstractTransition *transition)
{
  emit transitionTriggered(TransitionId(transition), Util::displayString(transition));
}

void StateMachineViewerServer::stateEntered(QAbstractState *state)
{
  emit message(tr("State entered: %1").arg(Util::displayString(state)));
  stateConfigurationChanged();
}

void StateMachineViewerServer::stateExited(QAbstractState *state)
{
  emit message(tr("State exited: %1").arg(Util::displayString(state)));
  stateConfigurationChanged();
}

void StateMachineViewerServer::stateConfigurationChanged()
{
  QSet<QAbstractState*> newConfig;
  if (selectedStateMachine()) {
    newConfig = selectedStateMachine()->configuration();
  }

  if (newConfig == m_lastStateConfig) {
    return;
  }
  m_lastStateConfig = newConfig;

  StateMachineConfiguration config;
  config.reserve(newConfig.size());
  foreach(QAbstractState* state, newConfig) {
    config << StateId(state);
  }

  emit stateConfigurationChanged(config);
}

void StateMachineViewerServer::addState(QAbstractState *state)
{
  if (!mayAddState(state)) {
    return;
  }

  Q_ASSERT(!m_recursionGuard.contains(state));
  m_recursionGuard.insert(state);

  QState *parentState = state->parentState();
  if (parentState) {
    addState(parentState); // be sure that parent is added first
  }

  const bool hasChildren = state->findChild<QAbstractState*>();
  const QString &label = Util::displayString(state);
  // add a connection from parent state to initial state if
  // parent state is valid and parent state has an initial state
  const bool connectToInitial = parentState && parentState->initialState() == state;
  StateType type = OtherState;
  if (qobject_cast<QFinalState*>(state)) {
    type = FinalState;
  } else if (auto historyState = qobject_cast<QHistoryState*>(state)) {
    type = historyState->historyType() == QHistoryState::ShallowHistory ? ShallowHistoryState : DeepHistoryState;
  } else if (qobject_cast<QStateMachine*>(state)) {
    type = StateMachineState;
  }

  emit stateAdded(StateId(state), StateId(parentState),
                  hasChildren, label, type, connectToInitial);

  // add transitions
  Q_FOREACH (QAbstractTransition *transition, state->findChildren<QAbstractTransition*>()) {
    addTransition(transition);
  }

  // recursive call to add children
  Q_FOREACH (QAbstractState* child, state->findChildren<QAbstractState*>()) {
    addState(child);
  }
}

void StateMachineViewerServer::addTransition(QAbstractTransition *transition)
{
  QState *sourceState = transition->sourceState();
  QAbstractState *targetState = transition->targetState();
  addState(sourceState);
  addState(targetState);

  const QString label = labelForTransition(transition);
  emit transitionAdded(TransitionId(transition), StateId(sourceState),
                       StateId(targetState), label);
}

void StateMachineViewerServer::updateStartStop()
{
  emit statusChanged(selectedStateMachine() != 0, selectedStateMachine() && selectedStateMachine()->isRunning());
}

void StateMachineViewerServer::toggleRunning()
{
  if (!selectedStateMachine()) {
    return;
  }
  if (selectedStateMachine()->isRunning()) {
    selectedStateMachine()->stop();
  } else {
    selectedStateMachine()->start();
  }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(StateMachineViewerFactory)
#endif
