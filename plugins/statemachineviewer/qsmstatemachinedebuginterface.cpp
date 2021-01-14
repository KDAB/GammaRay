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

#include "qsmstatemachinedebuginterface.h"

#include "statemachinewatcher.h"

#include <core/objectdataprovider.h>
#include <core/util.h>

#include <QAbstractTransition>
#include <QFinalState>
#include <QHistoryState>
#include <QMetaEnum>
#include <QSignalTransition>
#include <QState>
#include <QStateMachine>
#include <QStringList>

Q_DECLARE_METATYPE(Qt::KeyboardModifiers)

namespace GammaRay {

static QAbstractState *fromState(State state) {
    return reinterpret_cast<QAbstractState *>(static_cast<quintptr>(state));
}

static State toState(QAbstractState *state = nullptr) {
    return State(reinterpret_cast<quintptr>(state));
}

static QAbstractTransition *fromTransition(Transition transition) {
    return reinterpret_cast<QAbstractTransition *>(static_cast<quintptr>(transition));
}

static Transition toTransition(QAbstractTransition *transition) {
    return Transition(reinterpret_cast<quintptr>(transition));
}

template <typename T>
static QVector<T*> childrenOfType(QObject *parent) {
    Q_ASSERT(parent);
    QVector<T*> v;
    foreach (auto obj, parent->children()) {
        if (auto child = qobject_cast<T*>(obj))
            v.push_back(child);
    }
    return v;
}

QSMStateMachineDebugInterface::QSMStateMachineDebugInterface(QStateMachine *stateMachine, QObject *parent)
    : StateMachineDebugInterface(parent)
    , m_stateMachine(stateMachine)
    , m_stateMachineWatcher(new StateMachineWatcher(this))
{
    connect(stateMachine, &QStateMachine::started, this, &QSMStateMachineDebugInterface::updateRunning);
    connect(stateMachine, &QStateMachine::stopped, this, &QSMStateMachineDebugInterface::updateRunning);
    connect(stateMachine, &QState::finished, this, &QSMStateMachineDebugInterface::updateRunning);

    connect(m_stateMachineWatcher, &StateMachineWatcher::stateEntered, this, &StateMachineDebugInterface::stateEntered);
    connect(m_stateMachineWatcher, &StateMachineWatcher::stateExited, this, &StateMachineDebugInterface::stateExited);
    connect(m_stateMachineWatcher, &StateMachineWatcher::transitionTriggered, this, &StateMachineDebugInterface::transitionTriggered);

    m_stateMachineWatcher->setWatchedStateMachine(stateMachine);
}

QSMStateMachineDebugInterface::~QSMStateMachineDebugInterface() = default;

bool QSMStateMachineDebugInterface::isRunning() const
{
    return m_stateMachine->isRunning();
}

void QSMStateMachineDebugInterface::start()
{
    m_stateMachine->start();
}

void QSMStateMachineDebugInterface::stop()
{
    m_stateMachine->stop();
}

QVector<State> QSMStateMachineDebugInterface::configuration() const
{
    const QSet<QAbstractState *> configuration = m_stateMachine->configuration();

    QVector<State> result;
    result.reserve(configuration.size());

    for (QAbstractState *state : configuration) {
        result.append(toState(state));
    }

    std::sort(result.begin(), result.end());
    return result;
}

State QSMStateMachineDebugInterface::rootState() const
{
    return toState(m_stateMachine);
}

QVector<State> QSMStateMachineDebugInterface::stateChildren(State parentId) const
{
    QAbstractState *parent = fromState(parentId);

    if (!parent)
        parent = m_stateMachine;

    QVector<State> result;
    foreach (auto state, childrenOfType<QAbstractState>(parent)) {
        result.append(toState(state));
    }

    std::sort(result.begin(), result.end());
    return result;
}

State QSMStateMachineDebugInterface::parentState(State stateId) const
{
    QAbstractState *state = fromState(stateId);

    if (!state)
        return toState();

    return toState(state->parentState());
}

bool QSMStateMachineDebugInterface::isInitialState(State stateId) const
{
    QAbstractState *state = fromState(stateId);

    if (!state)
        return false;

    QState *parentState = state->parentState();
    return parentState && state == parentState->initialState();
}

QString QSMStateMachineDebugInterface::transitions(State stateId) const
{
    QState *state = qobject_cast<QState *>(fromState(stateId));

    if (!state)
        return QString();

    QState *parent = state->parentState() ? state->parentState() : m_stateMachine;
    const auto l = childrenOfType<QAbstractState>(parent);

    QStringList nums;
    const QList<QAbstractTransition *> trs = state->transitions();
    nums.reserve(trs.size());
    for (auto t : trs) {
        QAbstractState *child = t->targetState();
        nums << QString::number(l.indexOf(child) - l.indexOf(state));
    }
    return nums.join(QStringLiteral(","));
}

QString QSMStateMachineDebugInterface::stateLabel(State state) const
{
    return Util::displayString(fromState(state));
}

StateType QSMStateMachineDebugInterface::stateType(State stateId) const
{
    QAbstractState *state = fromState(stateId);
    StateType type = OtherState;
    if (qobject_cast<QFinalState *>(state)) {
        type = FinalState;
    } else if (auto historyState = qobject_cast<QHistoryState *>(state)) {
        type = historyState->historyType() == QHistoryState::ShallowHistory ? ShallowHistoryState :
                                                                              DeepHistoryState;
    } else if (qobject_cast<QStateMachine *>(state)) {
        type = StateMachineState;
    }
    return type;
}

QString QSMStateMachineDebugInterface::stateDisplay(State state) const
{
    return Util::shortDisplayString(fromState(state));
}

QString QSMStateMachineDebugInterface::stateDisplayType(State state) const
{
    return ObjectDataProvider::typeName(fromState(state));
}

QVector<Transition> QSMStateMachineDebugInterface::stateTransitions(State state) const
{
    QVector<Transition> result;

    foreach (auto object, fromState(state)->children()) {
        if (auto transition = qobject_cast<QAbstractTransition *>(object))
            result.append(toTransition(transition));
    }

    return result;
}

QString QSMStateMachineDebugInterface::transitionLabel(Transition t) const
{
    QAbstractTransition *transition = fromTransition(t);

    const QString objectName = transition->objectName();
    if (!objectName.isEmpty())
        return objectName;

    // try to find descriptive labels for built-in transitions
    if (auto signalTransition = qobject_cast<QSignalTransition *>(transition)) {
        QString str;
        if (signalTransition->senderObject() != transition->sourceState()) {
            str += Util::displayString(signalTransition->senderObject()) + "\n / ";
        }
        auto signal = signalTransition->signal();
        if (signal.startsWith('0' + QSIGNAL_CODE)) {
            // from QStateMachinePrivate::registerSignalTransition
            signal.remove(0, 1);
        }
        str += signal;
        return str;
    }
        // QKeyEventTransition is in QtWidgets, so this is a bit dirty to avoid a hard dependency
    else if (transition->inherits("QKeyEventTransition")) {
        QString s;
        const auto modifiers = transition->property("modifierMask").value<Qt::KeyboardModifiers>();
        if (modifiers != Qt::NoModifier) {
            const auto modIdx = staticQtMetaObject.indexOfEnumerator("KeyboardModifiers");
            if (modIdx < 0) {
                return Util::displayString(transition);
            }

            const auto modEnum = staticQtMetaObject.enumerator(modIdx);
            s += modEnum.valueToKey(modifiers) + QStringLiteral(" + ");
        }

        const auto key = transition->property("key").toInt();
        const auto keyIdx = staticQtMetaObject.indexOfEnumerator("Key");
        if (keyIdx < 0) {
            return Util::displayString(transition);
        }

        const auto keyEnum = staticQtMetaObject.enumerator(keyIdx);
        s += keyEnum.valueToKey(key);
        return s;
    }

    return Util::displayString(transition);
}

State QSMStateMachineDebugInterface::transitionSource(Transition t) const
{
    QAbstractTransition *transition = fromTransition(t);

    return toState(transition->sourceState());
}

QVector<State> QSMStateMachineDebugInterface::transitionTargets(Transition t) const
{
    QAbstractTransition *transition = fromTransition(t);

    return QVector<State>() << toState(transition->targetState());
}

void QSMStateMachineDebugInterface::updateRunning()
{
    emit runningChanged(m_stateMachine->isRunning());
}

bool QSMStateMachineDebugInterface::stateValid(State state) const
{
    return fromState(state) != nullptr;
}

QObject *QSMStateMachineDebugInterface::stateObject(State state) const
{
    return fromState(state);
}

}
