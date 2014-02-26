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

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEWATCHER_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEWATCHER_H

#include <QObject>
#include <QVector>

class QAbstractState;
class QAbstractState;
class QAbstractTransition;
class QState;
class QStateMachine;

namespace GammaRay {

class StateMachineWatcher : public QObject
{
  Q_OBJECT
  public:
    explicit StateMachineWatcher(QObject *parent = 0);
    virtual ~StateMachineWatcher();

    void setWatchedStateMachine(QStateMachine *machine);
    QStateMachine *watchedStateMachine() const;

  Q_SIGNALS:
    void stateEntered(QAbstractState *state);
    void stateExited(QAbstractState *state);

    void transitionTriggered(QAbstractTransition*);

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
