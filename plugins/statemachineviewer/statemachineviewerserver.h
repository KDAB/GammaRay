/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERSERVER_H
#define GAMMARAY_STATEMACHINEVIEWER_STATEMACHINEVIEWERSERVER_H

#include "statemachineviewerutil.h"
#include "statemachineviewerinterface.h"
#include "statemachinedebuginterface.h"

#include <core/toolfactory.h>

#include <QHash>
#include <QSet>
#include <QString>
#include <QVector>
#include <QStateMachine>

#include <config-gammaray.h>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QAbstractProxyModel;
class QItemSelectionModel;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class StateModel;
class TransitionModel;
class StateMachineDebugInterface;

class StateMachineViewerServer : public StateMachineViewerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::StateMachineViewerInterface)
public:
    explicit StateMachineViewerServer(Probe *probe, QObject *parent = nullptr);

    void addState(State state);
    void addTransition(Transition transition);

    StateMachineDebugInterface *selectedStateMachine() const;

    using StateMachineViewerInterface::stateConfigurationChanged;
private slots:
    void stateEntered(State state);
    void stateExited(State state);
    void stateConfigurationChanged();
    void handleTransitionTriggered(Transition transition);

    void stateSelectionChanged();

    void setFilteredStates(const QVector<State> &states);
    void selectStateMachine(int row) override;

    /// Takes ownership of @p machine
    void setSelectedStateMachine(StateMachineDebugInterface *machine);

    void updateStartStop();
    void toggleRunning() override;

    void repopulateGraph() override;

    void handleLogMessage(const QString &label, const QString &msg);

    void objectSelected(QObject *obj);

private:
    bool mayAddState(State state);

    QAbstractProxyModel *m_stateMachinesModel;
    StateModel *m_stateModel;
    QItemSelectionModel *m_stateSelectionModel;
    TransitionModel *m_transitionModel;

    // filters
    QVector<State> m_filteredStates;

    QVector<State> m_recursionGuard;
    QVector<State> m_lastStateConfig;
};

class StateMachineViewerFactory : public QObject,
    public StandardToolFactory<QStateMachine, StateMachineViewerServer>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_statemachineviewer.json")

public:
    explicit StateMachineViewerFactory(QObject *parent = nullptr);
};
}

#endif // GAMMARAY_STATEMACHINEVIEWERSERVER_H
