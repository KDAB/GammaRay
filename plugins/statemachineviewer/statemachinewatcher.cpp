/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "statemachinewatcher.h"

#include <QAbstractTransition>
#include <QFinalState>
#include <QState>
#include <QStateMachine>

#include <iostream>

using namespace GammaRay;

StateMachineWatcher::StateMachineWatcher(QObject *parent)
  : QObject(parent),
    m_watchedStateMachine(0),
    m_lastEnteredState(0),
    m_lastExitedState(0)
{
}

StateMachineWatcher::~StateMachineWatcher()
{
}

void StateMachineWatcher::setWatchedStateMachine(QStateMachine *machine)
{
  if (m_watchedStateMachine == machine) {
    return;
  }

  m_watchedStateMachine = machine;

  clearWatchedStates();
  Q_FOREACH (QAbstractState* state, machine->findChildren<QAbstractState*>()) {
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
  if (state->machine() != m_watchedStateMachine) {
    return;
  }

  connect(state, SIGNAL(entered()),
          this, SLOT(handleStateEntered()), Qt::UniqueConnection);
  connect(state, SIGNAL(exited()),
          this, SLOT(handleStateExited()), Qt::UniqueConnection);
  connect(state, SIGNAL(destroyed(QObject*)),
          this, SLOT(handleStateDestroyed()), Qt::UniqueConnection);

  Q_FOREACH (QAbstractTransition *transition, state->findChildren<QAbstractTransition*>()) {
    connect(transition, SIGNAL(triggered()),
            this, SLOT(handleTransitionTriggered()), Qt::UniqueConnection);
  }
  m_watchedStates << state;
}

void StateMachineWatcher::clearWatchedStates()
{
  Q_FOREACH (QAbstractState *state, m_watchedStates) {
    disconnect(state, SIGNAL(entered()), this, SLOT(handleStateEntered()));
    disconnect(state, SIGNAL(exited()), this, SLOT(handleStateExited()));
    disconnect(state, SIGNAL(destroyed(QObject*)), this, SLOT(handleStateDestroyed()));

    Q_FOREACH (QAbstractTransition *transition, state->findChildren<QAbstractTransition*>()) {
      disconnect(transition, SIGNAL(triggered()), this, SLOT(handleTransitionTriggered()));
    }
  }
  m_watchedStates.clear();
}

void StateMachineWatcher::handleTransitionTriggered()
{
  QAbstractTransition *transition = qobject_cast<QAbstractTransition*>(QObject::sender());
  Q_ASSERT(transition);

  emit transitionTriggered(transition);
}

void StateMachineWatcher::handleStateEntered()
{
  QAbstractState* state = qobject_cast<QAbstractState*>(QObject::sender());
  Q_ASSERT(state);

  if (state->machine() != m_watchedStateMachine) {
    return;
  }

  if (state == m_lastEnteredState) {
    return;
  }

  m_lastEnteredState = state;
  emit stateEntered(state);
}

void StateMachineWatcher::handleStateExited()
{
  QAbstractState* state = qobject_cast<QAbstractState*>(QObject::sender());
  Q_ASSERT(state);

  if (state->machine() != m_watchedStateMachine) {
    return;
  }

  if (state == m_lastExitedState) {
    return;
  }

  m_lastExitedState = state;
  emit stateExited(state);
}

void StateMachineWatcher::handleStateDestroyed()
{
  QAbstractState* state = static_cast<QAbstractState*>(QObject::sender());
  Q_ASSERT(state);

  const int index = m_watchedStates.indexOf(state);
  Q_ASSERT(index != -1);
  m_watchedStates.remove(index);
}
