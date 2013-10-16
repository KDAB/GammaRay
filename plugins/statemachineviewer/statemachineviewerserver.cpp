/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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


#include "include/objecttypefilterproxymodel.h"
#include "include/probeinterface.h"
#include <common/network/objectbroker.h>

#include <QAbstractTransition>
#include <QFinalState>
#include <QHistoryState>
#include <QSignalTransition>
#include <QStateMachine>
#include <QItemSelectionModel>
#include <QLabel>

#include <QtPlugin>

#include <iostream>

using namespace GammaRay;
using namespace std;

StateMachineViewerServer::StateMachineViewerServer(ProbeInterface *probe, QObject *parent)
  : StateMachineViewerInterface(parent),
    m_stateModel(new StateModel(this)),
    m_transitionModel(new TransitionModel(this)),
    m_filteredState(0),
    m_maximumDepth(0),
    m_stateMachineWatcher(new StateMachineWatcher(this))
{
  probe->registerModel("com.kdab.GammaRay.StateModel", m_stateModel);
  QItemSelectionModel *stateSelectionModel = ObjectBroker::selectionModel(m_stateModel);
  connect(stateSelectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(handleStateClicked(QModelIndex)));

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

  addState(m_filteredState ? m_filteredState : m_stateModel->stateMachine());
  m_recursionGuard.clear();

  emit graphRepopulated();
}

QStateMachine *StateMachineViewerServer::selectedStateMachine() const
{
  return m_stateModel->stateMachine();
}

int treeDepth(QAbstractState *ascendant, QAbstractState *obj)
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

  if (m_filteredState) {
    if (m_filteredState != state && !Util::descendantOf(m_filteredState, state)) {
      return false;
    }
  }

  if (m_maximumDepth > 0) {
    if (::treeDepth(m_filteredState, state) > m_maximumDepth) {
      return false;
    }
  }

  return true;
}

void StateMachineViewerServer::setFilteredState(QAbstractState *state)
{
  if (m_filteredState == state) {
    return;
  }

  emit message(tr("Setting filter on: %1").arg(Util::displayString(state)));
  m_filteredState = state;
  repopulateGraph();
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

void StateMachineViewerServer::handleMachineClicked(const QModelIndex &index)
{
  QObject *stateMachineObject = index.data(ObjectModel::ObjectRole).value<QObject*>();
  QStateMachine *machine = qobject_cast<QStateMachine*>(stateMachineObject);
  Q_ASSERT(machine);

  m_stateModel->setStateMachine(machine);
  stateConfigurationChanged();

  setFilteredState(machine);
  m_stateMachineWatcher->setWatchedStateMachine(machine);

  connect(machine, SIGNAL(started()), SLOT(updateStartStop()), Qt::UniqueConnection);
  connect(machine, SIGNAL(stopped()), SLOT(updateStartStop()), Qt::UniqueConnection);
  connect(machine, SIGNAL(finished()),SLOT(updateStartStop()), Qt::UniqueConnection);
  updateStartStop();
}

void StateMachineViewerServer::handleStateClicked(const QModelIndex &index)
{
  QObject *stateObject = index.data(ObjectModel::ObjectRole).value<QObject*>();
  Q_ASSERT(stateObject);
  QAbstractState *state = qobject_cast<QAbstractState*>(stateObject);
  Q_ASSERT(state);
  setFilteredState(state);
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
  } else if (qobject_cast<QHistoryState*>(state)) {
    type = HistoryState;
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

  QString label = transition->objectName();
  if (label.isEmpty()) {
    // Try to get a label for the transition if it is a QSignalTransition.
    QSignalTransition *signalTransition = qobject_cast<QSignalTransition*>(transition);
    if (signalTransition) {
        label = QString::fromLatin1("%1::%2").
                    arg(Util::displayString(signalTransition->senderObject())).
                    arg(QString::fromLatin1(signalTransition->signal().mid(1)));
    } else {
      label = Util::displayString(transition);
    }
  }

  emit transitionAdded(TransitionId(transition), StateId(sourceState),
                       StateId(targetState), label);
}

void StateMachineViewerServer::updateStartStop()
{
  emit statusChanged(selectedStateMachine(), selectedStateMachine() && selectedStateMachine()->isRunning());
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

#ifndef HAVE_GRAPHVIZ
QWidget *StateMachineViewerFactory::createWidget(QWidget *parentWidget)
{
  ///FIXME: properly split up server/client and make this obsolete
  QLabel *label = new QLabel(parentWidget);
  label->setAlignment(Qt::AlignCenter);
  label->setText(tr("GammaRay was built without GraphViz support. State Machine visualization is disabled."));
  return label;
}
#endif

#include "statemachineviewerserver.moc"
