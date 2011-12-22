/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tobias Koenig <tobias.koenig@kdab.com>

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
#ifndef GAMMARAY_STATEMACHINEDUMPER_H
#define GAMMARAY_STATEMACHINEDUMPER_H

#include "gvgraph/gvgraph.h"

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QStateMachine>

class StateMachineDumper : public QObject
{
  public:
    explicit StateMachineDumper(QObject *parent = 0);

    void dump(QStateMachine *stateMachine, const QString &baseDirectory);

  private:
    void dumpState(QAbstractState *state) const;

    void collectStates(QAbstractState *parentState, QSet<QAbstractState*> &states);

    /**
     * Returns @c true if @p state has at least on child state and all child states are leaf states.
     */
    bool isLeafParentState(const QState *state) const;

    /**
     * Returns all outgoing transitions of the @p state or its child states.
     */
    QSet<QAbstractTransition*> outgoingTransitions(const QState *state) const;

    void addNode(QAbstractState *state, GammaRay::GVGraph *graph, GammaRay::GraphId graphId, QHash<QAbstractState*, GammaRay::NodeId> &nodeMap, bool isParentState) const;
    void addTransition(QAbstractState *sourceState, QAbstractTransition *transition, GammaRay::GVGraph *graph, QHash<QAbstractState*, GammaRay::NodeId> &nodeMap, bool isParentState) const;
    void exportState(QState *state, const QString &fileName) const;

    QStateMachine *m_stateMachine;
    QSet<QAbstractState*> m_allStates;
    QString m_baseDirectory;
};

#endif
