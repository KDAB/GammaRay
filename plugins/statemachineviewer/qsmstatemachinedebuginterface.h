/*
  qsmstatemachinedebuginterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Arne Petersen <jan.petersen@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STATEMACHINEVIEWER_QSMSTATEMACHINEDEBUGINTERFACE_H
#define GAMMARAY_STATEMACHINEVIEWER_QSMSTATEMACHINEDEBUGINTERFACE_H

#include "statemachinedebuginterface.h"

QT_BEGIN_NAMESPACE
class QAbstractState;
class QAbstractTransition;
class QStateMachine;
QT_END_NAMESPACE

namespace GammaRay {

class StateMachineWatcher;

class QSMStateMachineDebugInterface : public StateMachineDebugInterface
{
    Q_OBJECT

public:
    explicit QSMStateMachineDebugInterface(QStateMachine *stateMachine, QObject *parent = nullptr);
    ~QSMStateMachineDebugInterface() override;

    bool isRunning() const override;

    void start() override;
    void stop() override;

    QVector<State> configuration() const override;

    State rootState() const override;
    bool stateValid(State state) const override;
    QVector<State> stateChildren(State parentId) const override;
    State parentState(State stateId) const override;

    bool isInitialState(State stateId) const override;
    QString transitions(State stateId) const override;

    QString stateLabel(State state) const override;
    StateType stateType(State stateId) const override;
    QObject *stateObject(State state) const override;

    QVector<Transition> stateTransitions(State state) const override;
    QString transitionLabel(Transition t) const override;
    State transitionSource(Transition t) const override;
    QVector<State> transitionTargets(Transition t) const override;
    QString stateDisplay(State state) const override;
    QString stateDisplayType(State state) const override;

private slots:
    void updateRunning();

private:
    QStateMachine *m_stateMachine;

    StateMachineWatcher *m_stateMachineWatcher;
};
}

#endif
