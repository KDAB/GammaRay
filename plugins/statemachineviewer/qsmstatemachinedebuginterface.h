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
