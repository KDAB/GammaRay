/*
  qscxmlstatemachinedebuginterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Jan Arne Petersen <jan.petersen@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_STATEMACHINEVIEWER_QSCXMLSTATEMACHINEDEBUGINTERFACE_H
#define GAMMARAY_STATEMACHINEVIEWER_QSCXMLSTATEMACHINEDEBUGINTERFACE_H

#include "statemachinedebuginterface.h"

#include <private/qscxmlstatemachineinfo_p.h>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QScxmlStateMachine;
class QScxmlStateMachineInfo;
QT_END_NAMESPACE

namespace GammaRay {

class QScxmlStateMachineDebugInterface : public StateMachineDebugInterface
{
    Q_OBJECT

public:
    explicit QScxmlStateMachineDebugInterface(QScxmlStateMachine *stateMachine, QObject *parent = nullptr);
    ~QScxmlStateMachineDebugInterface();

    bool isRunning() const override;
    void start() override;
    void stop() override;

    QVector<State> configuration() const override;
    State rootState() const override;
    bool stateValid(State state) const override;
    QVector<State> stateChildren(State state) const override;
    State parentState(State state) const override;
    bool isInitialState(State state) const override;
    QString transitions(State state) const override;
    QString stateLabel(State state) const override;
    QString stateDisplay(State state) const override;
    QString stateDisplayType(State state) const override;
    StateType stateType(State state) const override;
    QObject *stateObject(State state) const override;
    QVector<Transition> stateTransitions(State state) const override;
    QString transitionLabel(Transition transition) const override;
    State transitionSource(Transition transition) const override;
    QVector<State> transitionTargets(Transition transition) const override;

private slots:
    void statesEntered(const QVector<QScxmlStateMachineInfo::StateId> &states);
    void statesExited(const QVector<QScxmlStateMachineInfo::StateId> &states);
    void transitionsTriggered(const QVector<QScxmlStateMachineInfo::TransitionId> &transitions);

private:
    QScxmlStateMachine *m_stateMachine;
    QPointer<QScxmlStateMachineInfo> m_info;
};
}

#endif
