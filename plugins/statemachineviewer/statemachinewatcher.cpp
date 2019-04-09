/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "statemachinewatcher.h"

#include <compat/qasconst.h>

#include <QAbstractTransition>
#include <QFinalState>
#include <QState>
#include <QStateMachine>

#include <iostream>

using namespace GammaRay;

static State toState(QAbstractState *state = nullptr) {
    return State(reinterpret_cast<quintptr>(state));
}

static Transition toTransition(QAbstractTransition *transition) {
    return Transition(reinterpret_cast<quintptr>(transition));
}

StateMachineWatcher::StateMachineWatcher(QObject *parent)
    : QObject(parent)
    , m_watchedStateMachine(nullptr)
    , m_lastEnteredState(nullptr)
    , m_lastExitedState(nullptr)
{
}

StateMachineWatcher::~StateMachineWatcher() = default;

void StateMachineWatcher::setWatchedStateMachine(QStateMachine *machine)
{
    if (m_watchedStateMachine == machine)
        return;

    m_watchedStateMachine = machine;

    clearWatchedStates();
    Q_FOREACH(QAbstractState* state, machine->findChildren<QAbstractState *>()) {
        watchState(state);
    }

    emit watchedStateMachineChanged(machine);
}

QStateMachine *StateMachineWatcher::watchedStateMachine() const
{
    return m_watchedStateMachine;
}

void StateMachineWatcher::watchState(QAbstractState *state)
{
    if (state->machine() != m_watchedStateMachine)
        return;

    connect(state, &QAbstractState::entered,
            this, &StateMachineWatcher::handleStateEntered, Qt::UniqueConnection);
    connect(state, &QAbstractState::exited,
            this, &StateMachineWatcher::handleStateExited, Qt::UniqueConnection);
    connect(state, &QObject::destroyed,
            this, &StateMachineWatcher::handleStateDestroyed, Qt::UniqueConnection);

    Q_FOREACH(QAbstractTransition *transition, state->findChildren<QAbstractTransition *>()) {
        connect(transition, &QAbstractTransition::triggered,
                this, &StateMachineWatcher::handleTransitionTriggered, Qt::UniqueConnection);
    }
    m_watchedStates << state;
}

void StateMachineWatcher::clearWatchedStates()
{
    for (QAbstractState *state : qAsConst(m_watchedStates)) {
        disconnect(state, &QAbstractState::entered, this, &StateMachineWatcher::handleStateEntered);
        disconnect(state, &QAbstractState::exited, this, &StateMachineWatcher::handleStateExited);
        disconnect(state, &QObject::destroyed, this, &StateMachineWatcher::handleStateDestroyed);

        Q_FOREACH(QAbstractTransition *transition, state->findChildren<QAbstractTransition *>()) {
            disconnect(transition, &QAbstractTransition::triggered, this, &StateMachineWatcher::handleTransitionTriggered);
        }
    }
    m_watchedStates.clear();
}

void StateMachineWatcher::handleTransitionTriggered()
{
    QAbstractTransition *transition = qobject_cast<QAbstractTransition *>(QObject::sender());
    Q_ASSERT(transition);

    emit transitionTriggered(toTransition(transition));
}

void StateMachineWatcher::handleStateEntered()
{
    QAbstractState *state = qobject_cast<QAbstractState *>(QObject::sender());
    Q_ASSERT(state);

    if (state->machine() != m_watchedStateMachine)
        return;

    if (state == m_lastEnteredState)
        return;

    m_lastEnteredState = state;
    emit stateEntered(toState(state));
}

void StateMachineWatcher::handleStateExited()
{
    QAbstractState *state = qobject_cast<QAbstractState *>(QObject::sender());
    Q_ASSERT(state);

    if (state->machine() != m_watchedStateMachine)
        return;

    if (state == m_lastExitedState)
        return;

    m_lastExitedState = state;
    emit stateExited(toState(state));
}

void StateMachineWatcher::handleStateDestroyed()
{
    QAbstractState *state = static_cast<QAbstractState *>(QObject::sender());
    Q_ASSERT(state);

    const int index = m_watchedStates.indexOf(state);
    Q_ASSERT(index != -1);
    m_watchedStates.remove(index);
}
