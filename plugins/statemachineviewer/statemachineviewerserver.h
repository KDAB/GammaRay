/*
  statemachineviewerserver.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    void stateEntered(GammaRay::State state);
    void stateExited(GammaRay::State state);
    void stateConfigurationChanged();
    void handleTransitionTriggered(GammaRay::Transition transition);

    void stateSelectionChanged();

    void setFilteredStates(const QVector<GammaRay::State> &states);
    void selectStateMachine(int row) override;

    /// Takes ownership of @p machine
    void setSelectedStateMachine(GammaRay::StateMachineDebugInterface *machine);

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
