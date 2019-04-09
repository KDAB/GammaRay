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
    void stateEntered(State state);
    void stateExited(State state);

    void transitionTriggered(Transition transition);

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
