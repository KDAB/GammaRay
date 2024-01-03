/*
  statemachinewatcher.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEWATCHER_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEWATCHER_H

#include <QObject>
#include <QVector>

#include "statemachinedebuginterface.h"

QT_BEGIN_NAMESPACE
class QAbstractState;
class QAbstractState;
class QAbstractTransition;
class QState;
class QStateMachine;
QT_END_NAMESPACE

namespace GammaRay {
class StateMachineWatcher : public QObject
{
    Q_OBJECT
public:
    explicit StateMachineWatcher(QObject *parent = nullptr);
    ~StateMachineWatcher() override;

    void setWatchedStateMachine(QStateMachine *machine);
    QStateMachine *watchedStateMachine() const;

Q_SIGNALS:
    void stateEntered(GammaRay::State state);
    void stateExited(GammaRay::State state);

    void transitionTriggered(GammaRay::Transition transition);

    void watchedStateMachineChanged(QStateMachine *);

private Q_SLOTS:
    void watchState(QAbstractState *state);
    void clearWatchedStates();

    void handleStateEntered();
    void handleStateExited();
    void handleStateDestroyed();
    void handleTransitionTriggered();

private:
    QStateMachine *m_watchedStateMachine;
    QVector<QAbstractState *> m_watchedStates;

    QAbstractState *m_lastEnteredState;
    QAbstractState *m_lastExitedState;
};
}

#endif // GAMMARAY_STATEMACHINEWATCHER_H
