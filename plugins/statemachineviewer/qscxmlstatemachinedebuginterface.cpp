/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Jan Arne Petersen <jan.petersen@kdab.com>

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

#include "qscxmlstatemachinedebuginterface.h"

#include <QScxmlStateMachine>
#include <private/qscxmlstatemachineinfo_p.h>

namespace GammaRay {

static QScxmlStateMachineInfo::StateId fromState(State state) {
    return static_cast<QScxmlStateMachineInfo::StateId>(state);
}

static State toState(QScxmlStateMachineInfo::StateId state) {
    return State(static_cast<quintptr>(state));
}

static QScxmlStateMachineInfo::TransitionId fromTransition(Transition transition) {
    return static_cast<QScxmlStateMachineInfo::TransitionId>(transition);
}

static Transition toTransition(QScxmlStateMachineInfo::TransitionId transition) {
    return Transition(static_cast<quintptr>(transition));
}

QScxmlStateMachineDebugInterface::QScxmlStateMachineDebugInterface(QScxmlStateMachine *stateMachine, QObject *parent)
    : StateMachineDebugInterface(parent)
    , m_stateMachine(stateMachine)
    , m_info(new QScxmlStateMachineInfo(stateMachine))
{
    connect(stateMachine, SIGNAL(runningChanged(bool)), this, SIGNAL(runningChanged(bool)));

    connect(stateMachine, SIGNAL(log(QString,QString)), this, SIGNAL(logMessage(QString,QString)));

    connect(m_info.data(), SIGNAL(statesEntered(QVector<QScxmlStateMachineInfo::StateId>)), this, SLOT(statesEntered(QVector<QScxmlStateMachineInfo::StateId>)));
    connect(m_info.data(), SIGNAL(statesExited(QVector<QScxmlStateMachineInfo::StateId>)), this, SLOT(statesExited(QVector<QScxmlStateMachineInfo::StateId>)));
    connect(m_info.data(), SIGNAL(transitionsTriggered(QVector<QScxmlStateMachineInfo::TransitionId>)), this, SLOT(transitionsTriggered(QVector<QScxmlStateMachineInfo::TransitionId>)));
}

QScxmlStateMachineDebugInterface::~QScxmlStateMachineDebugInterface()
{
    delete m_info;
}

bool QScxmlStateMachineDebugInterface::isRunning() const
{
    return m_stateMachine->isRunning();
}

void QScxmlStateMachineDebugInterface::start()
{
    m_stateMachine->start();
}

void QScxmlStateMachineDebugInterface::stop()
{
    m_stateMachine->stop();
}

QVector<State> QScxmlStateMachineDebugInterface::configuration() const
{
    const QVector<QScxmlStateMachineInfo::StateId> configuration = m_info->configuration();

    QVector<State> result;
    result.reserve(configuration.size());

    for (auto stateId : configuration)
        result.append(toState(stateId));

    std::sort(result.begin(), result.end());
    return result;
}

State QScxmlStateMachineDebugInterface::rootState() const
{
    return toState(QScxmlStateMachineInfo::InvalidStateId);
}

QVector<State> QScxmlStateMachineDebugInterface::stateChildren(State state) const
{
    const QVector<QScxmlStateMachineInfo::StateId> children = m_info->stateChildren(fromState(state));

    QVector<State> result;
    result.reserve(children.size());

    for (auto stateId : children)
        result.append(toState(stateId));

    return result;
}

State QScxmlStateMachineDebugInterface::parentState(State state) const
{
    return toState(m_info->stateParent(fromState(state)));
}

bool QScxmlStateMachineDebugInterface::isInitialState(State state) const
{
    const auto stateId = fromState(state);

    const auto parentState = m_info->stateParent(state);
    const auto parentInitialTransition = m_info->initialTransition(parentState);
    if (parentInitialTransition == QScxmlStateMachineInfo::InvalidTransitionId)
        return false;

    return m_info->transitionTargets(parentInitialTransition).contains(stateId);
}

QString QScxmlStateMachineDebugInterface::transitions(State state) const
{
    Q_UNUSED(state);

    return QString(); // FIXME
}

QString QScxmlStateMachineDebugInterface::stateLabel(State state) const
{
    auto stateId = fromState(state);
    if (stateId == QScxmlStateMachineInfo::InvalidStateId)
        return m_stateMachine->name();

    return QStringLiteral("%1 (%2)").arg(m_info->stateName(stateId)).arg(stateId);
}

QString QScxmlStateMachineDebugInterface::stateDisplay(State state) const
{
    return stateLabel(state);
}

QString QScxmlStateMachineDebugInterface::stateDisplayType(State state) const
{
    switch (m_info->stateType(fromState(state))) {
        case QScxmlStateMachineInfo::InvalidState:
            return QStringLiteral("StateMachineState");
        case QScxmlStateMachineInfo::NormalState:
            return QStringLiteral("NormalState");
        case QScxmlStateMachineInfo::ParallelState:
            return QStringLiteral("ParallelState");
        case QScxmlStateMachineInfo::FinalState:
            return QStringLiteral("FinalState");
        case QScxmlStateMachineInfo::ShallowHistoryState:
            return QStringLiteral("ShallowHistoryState");
        case QScxmlStateMachineInfo::DeepHistoryState:
            return QStringLiteral("DeepHistoryState");
    }

    return QString();
}

StateType QScxmlStateMachineDebugInterface::stateType(State state) const
{
    switch (m_info->stateType(fromState(state))) {
        case QScxmlStateMachineInfo::InvalidState:
            return StateMachineState;
        case QScxmlStateMachineInfo::NormalState:
            return OtherState;
        case QScxmlStateMachineInfo::ParallelState:
            return OtherState; // FIXME: No ParallelState. Bug.
        case QScxmlStateMachineInfo::FinalState:
            return FinalState;
        case QScxmlStateMachineInfo::ShallowHistoryState:
            return ShallowHistoryState;
        case QScxmlStateMachineInfo::DeepHistoryState:
            return DeepHistoryState;
    }

    return OtherState;
}

QVector<Transition> QScxmlStateMachineDebugInterface::stateTransitions(State state) const
{
    const auto stateId = fromState(state);
    const auto transitions = m_info->allTransitions();

    const auto initialTransition = m_info->initialTransition(stateId);

    QVector<Transition> result;
    for (auto transition : transitions) {
        if (transition != initialTransition && m_info->transitionSource(transition) == stateId)
            result.append(toTransition(transition));
    }

    return result;
}

QString QScxmlStateMachineDebugInterface::transitionLabel(Transition transition) const
{
    auto transitionId = fromTransition(transition);

    if (transitionId == QScxmlStateMachineInfo::InvalidTransitionId) {
        return QString();
    }

    auto events = m_info->transitionEvents(transition);
    if (events.empty()) {
        return QString();
    }

    return QStringLiteral("%1 (%2)").arg(events.first()).arg(transition);
}

State QScxmlStateMachineDebugInterface::transitionSource(Transition transition) const
{
    return toState(m_info->transitionSource(fromTransition(transition)));
}

QVector<State> QScxmlStateMachineDebugInterface::transitionTargets(Transition transition) const
{
    const auto targets = m_info->transitionTargets(fromTransition(transition));

    QVector<State> result;
    result.reserve(targets.size());

    for (auto stateId : targets)
        result.append(toState(stateId));

    return result;
}

void QScxmlStateMachineDebugInterface::statesEntered(const QVector<QScxmlStateMachineInfo::StateId> &states)
{
    for (auto state : states) {
        emit stateEntered(toState(state));
    }
}

void QScxmlStateMachineDebugInterface::statesExited(const QVector<QScxmlStateMachineInfo::StateId> &states)
{
    for (auto state : states) {
        emit stateExited(toState(state));
    }
}

void QScxmlStateMachineDebugInterface::transitionsTriggered(const QVector<QScxmlStateMachineInfo::TransitionId> &transitions)
{
    for (auto transition : transitions) {
        emit transitionTriggered(toTransition(transition));
    }
}

bool QScxmlStateMachineDebugInterface::stateValid(State state) const
{
    Q_UNUSED(state);
    return true;
}

QObject *QScxmlStateMachineDebugInterface::stateObject(State state) const
{
    Q_UNUSED(state);
    return m_stateMachine;
}

}
