/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "statemachineviewerserver.h"

#include "qsmstatemachinedebuginterface.h"
#ifdef HAVE_QT_SCXML
#include "qscxmlstatemachinedebuginterface.h"
#endif
#include "statemodel.h"
#include "statemachinedebuginterface.h"
#include "statemachinewatcher.h"
#include "transitionmodel.h"

#include <core/objecttypefilterproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/remote/serverproxymodel.h>
#include <common/objectbroker.h>
#include <compat/qasconst.h>

#include <QStateMachine>
#include <QItemSelectionModel>

#ifdef HAVE_QT_SCXML
#include <QScxmlStateMachine>
#endif

#include <QtPlugin>

#include <iostream>

using namespace GammaRay;
using namespace std;

StateMachineViewerServer::StateMachineViewerServer(Probe *probe, QObject *parent)
    : StateMachineViewerInterface(parent)
    , m_stateModel(new StateModel(this))
    , m_transitionModel(new TransitionModel(this))
{
    auto proxyModel = new ServerProxyModel<QIdentityProxyModel>(this);
    proxyModel->setSourceModel(m_stateModel);
    proxyModel->addRole(StateModel::StateIdRole);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.StateModel"), proxyModel);
    m_stateSelectionModel = ObjectBroker::selectionModel(proxyModel);
    connect(m_stateSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &StateMachineViewerServer::stateSelectionChanged);
    connect(probe, &Probe::objectSelected, this, &StateMachineViewerServer::objectSelected);

#ifdef HAVE_QT_SCXML
    auto stateMachineFilter = new ObjectTypeFilterProxyModel<QStateMachine, QScxmlStateMachine>(this);
#else
    auto stateMachineFilter = new ObjectTypeFilterProxyModel<QStateMachine>(this);
#endif
    stateMachineFilter->setSourceModel(probe->objectListModel());
    m_stateMachinesModel = new ServerProxyModel<SingleColumnObjectProxyModel>(this);
    m_stateMachinesModel->setSourceModel(stateMachineFilter);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.StateMachineModel"),
                         m_stateMachinesModel);

    updateStartStop();
}

void StateMachineViewerServer::repopulateGraph()
{
    if (!m_stateModel->stateMachine())
        return;

    emit aboutToRepopulateGraph();

    // just to be sure the client has the same setting than we do
    updateStartStop();

    if (m_filteredStates.isEmpty()) {
        addState(m_stateModel->stateMachine()->rootState());
    } else {
        for (State state : qAsConst(m_filteredStates))
            addState(state);
    }
    m_recursionGuard.clear();

    emit graphRepopulated();
}

StateMachineDebugInterface *StateMachineViewerServer::selectedStateMachine() const
{
    return m_stateModel->stateMachine();
}

bool StateMachineViewerServer::mayAddState(State state)
{
    if (!selectedStateMachine()->stateValid(state))
        return false;

    if (m_recursionGuard.contains(state))
        return false;

    for (State filter : qAsConst(m_filteredStates)) {
        if (filter == state || selectedStateMachine()->isDescendantOf(filter, state)) {
            return true;
        }
    }

    return m_filteredStates.isEmpty();
}

void StateMachineViewerServer::setFilteredStates(const QVector<State> &states)
{
    if (m_filteredStates == states)
        return;

    if (states.isEmpty()) {
        emit message(tr("Clearing filter."));
    } else {
        QStringList stateNames;
        stateNames.reserve(states.size());
        for (State state : states)
            stateNames << selectedStateMachine()->stateLabel(state);
        emit message(tr("Setting filter on: %1").arg(stateNames.join(QStringLiteral(", "))));
    }

    m_filteredStates = states;
}

void StateMachineViewerServer::setSelectedStateMachine(StateMachineDebugInterface *machine)
{
    StateMachineDebugInterface *oldMachine = selectedStateMachine();
    if (oldMachine == machine)
        return;

    if (oldMachine) {
        oldMachine->disconnect(this);
    }

    m_stateModel->setStateMachine(machine);

    setFilteredStates(QVector<State>());

    repopulateGraph();
    stateConfigurationChanged();

    if (machine) {
        machine->setParent(this);

        connect(machine, &StateMachineDebugInterface::runningChanged, this, &StateMachineViewerServer::updateStartStop);
        connect(machine, &StateMachineDebugInterface::stateEntered, this, &StateMachineViewerServer::stateEntered);
        connect(machine, &StateMachineDebugInterface::stateExited, this, &StateMachineViewerServer::stateExited);
        connect(machine, &StateMachineDebugInterface::transitionTriggered, this, &StateMachineViewerServer::handleTransitionTriggered);
        connect(machine, &StateMachineDebugInterface::logMessage, this, &StateMachineViewerServer::handleLogMessage);
    }
    updateStartStop();

    delete oldMachine;
}

void StateMachineViewerServer::selectStateMachine(int row)
{
    Q_ASSERT(m_stateMachinesModel);
    const auto index = m_stateMachinesModel->index(row, 0);
    if (!index.isValid()) {
        setSelectedStateMachine(nullptr);
        return;
    }

    QObject *stateMachineObject = index.data(ObjectModel::ObjectRole).value<QObject *>();
    QStateMachine *machine = qobject_cast<QStateMachine *>(stateMachineObject);
    if (machine) {
        setSelectedStateMachine(new QSMStateMachineDebugInterface(machine, this));
        return;
    }
#ifdef HAVE_QT_SCXML
    QScxmlStateMachine *qscxmlMachine = qobject_cast<QScxmlStateMachine *>(stateMachineObject);
    if (qscxmlMachine) {
        setSelectedStateMachine(new QScxmlStateMachineDebugInterface(qscxmlMachine, this));
        return;
    }
#endif
    setSelectedStateMachine(nullptr);
}

void StateMachineViewerServer::stateSelectionChanged()
{
    const QModelIndexList &selection = m_stateSelectionModel->selectedRows();
    qDebug() << selection;
    QVector<State> filter;
    filter.reserve(selection.size());
    for (const QModelIndex &index : selection) {
        State state = index.data(StateModel::StateValueRole).value<State>();
        bool addState = true;
        /// only pick the top-level items of the selection
        // NOTE: this might be slow for large selections, if someone wants to come up with a better
        // algorithm, please - go for it!
        for (State potentialParent : qAsConst(filter)) {
            if (selectedStateMachine()->isDescendantOf(potentialParent, state)) {
                addState = false;
                break;
            }
        }

        if (addState)
            filter << state;
    }
    setFilteredStates(filter);
}

void StateMachineViewerServer::handleTransitionTriggered(Transition transition)
{
    emit transitionTriggered(TransitionId(transition), selectedStateMachine()->transitionLabel(transition));
}

void StateMachineViewerServer::stateEntered(State state)
{
    emit message(tr("State entered: %1").arg(selectedStateMachine()->stateLabel(state)));
    stateConfigurationChanged();
}

void StateMachineViewerServer::stateExited(State state)
{
    emit message(tr("State exited: %1").arg(selectedStateMachine()->stateLabel(state)));
    stateConfigurationChanged();
}

void StateMachineViewerServer::stateConfigurationChanged()
{
    QVector<State> newConfig;
    if (selectedStateMachine())
        newConfig = selectedStateMachine()->configuration();

    if (newConfig == m_lastStateConfig)
        return;
    m_lastStateConfig = newConfig;

    StateMachineConfiguration config;
    config.reserve(newConfig.size());
    for (State state : qAsConst(newConfig))
        config << StateId(state);

    emit stateConfigurationChanged(config);
}

void StateMachineViewerServer::addState(State state)
{
    if (!selectedStateMachine()->stateValid(state))
        return;

    if (!mayAddState(state))
        return;

    Q_ASSERT(!m_recursionGuard.contains(state));
    m_recursionGuard.append(state);

    State parentState = selectedStateMachine()->parentState(state);
    addState(parentState); // be sure that parent is added first

    const bool hasChildren = !selectedStateMachine()->stateChildren(state).isEmpty();
    const QString &label = selectedStateMachine()->stateLabel(state);
    // add a connection from parent state to initial state if
    // parent state is valid and parent state has an initial state
    const bool connectToInitial = parentState && selectedStateMachine()->isInitialState(state);
    StateType type = selectedStateMachine()->stateType(state);

    emit stateAdded(StateId(state), StateId(parentState),
                    hasChildren, label, type, connectToInitial);

    // add outgoing transitions
    Q_FOREACH(auto transition, selectedStateMachine()->stateTransitions(state)) {
        addTransition(transition);
    }

    // add sub-states
    Q_FOREACH(auto childState, selectedStateMachine()->stateChildren(state)) {
        addState(childState);
    }
}

void StateMachineViewerServer::addTransition(Transition transition)
{
    const QString label = selectedStateMachine()->transitionLabel(transition);
    const State sourceState = selectedStateMachine()->transitionSource(transition);

    addState(sourceState);

    foreach (auto targetState, selectedStateMachine()->transitionTargets(transition)) {
        addState(targetState);

        emit transitionAdded(TransitionId(transition), StateId(sourceState),
                             StateId(targetState), label);
    }
}

void StateMachineViewerServer::updateStartStop()
{
    emit statusChanged(selectedStateMachine() != nullptr,
                       selectedStateMachine() && selectedStateMachine()->isRunning());
}

void StateMachineViewerServer::toggleRunning()
{
    if (!selectedStateMachine())
        return;
    if (selectedStateMachine()->isRunning())
        selectedStateMachine()->stop();
    else
        selectedStateMachine()->start();
}

void StateMachineViewerServer::handleLogMessage(const QString &label, const QString &msg)
{
    emit message(tr("Log [label=%1]: %2").arg(label, msg));
}

void StateMachineViewerServer::objectSelected(QObject *obj)
{
    if (auto state = qobject_cast<QAbstractState*>(obj)) {
        auto model = m_stateSelectionModel->model();
        const auto idxs = model->match(model->index(0, 0), StateModel::StateValueRole,
                                       QVariant::fromValue(GammaRay::State(quintptr(state))), 1, Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
        qDebug() << idxs;
        if (idxs.isEmpty())
            return;
        const auto idx = idxs.first();
        m_stateSelectionModel->select(idx, QItemSelectionModel::ClearAndSelect |
            QItemSelectionModel::Rows | QItemSelectionModel::Current);
    }
}

StateMachineViewerFactory::StateMachineViewerFactory(QObject *parent)
    : QObject(parent)
{
    setSupportedTypes(QVector<QByteArray>() << QByteArrayLiteral("QStateMachine")
                                            << QByteArrayLiteral("QScxmlStateMachine"));
}
