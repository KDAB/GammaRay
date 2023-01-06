/*
  statemachineviewerwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef STATEMACHINEVIEWERWIDGETNG_H
#define STATEMACHINEVIEWERWIDGETNG_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>
#include "statemachineviewerinterface.h"

#include <QWidget>

namespace KDSME {
class State;
class StateMachine;
class StateMachineView;
class Transition;
}

namespace GammaRay {
class DeferredTreeView;

namespace Ui {
class StateMachineViewerWidget;
}

class StateMachineViewerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StateMachineViewerWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~StateMachineViewerWidget();

    Q_INVOKABLE void saveTargetState(QSettings *settings) const;
    Q_INVOKABLE void restoreTargetState(QSettings *settings);

    KDSME::StateMachineView *stateMachineView() const;
    DeferredTreeView *objectInspector() const;

private slots:
    void showMessage(const QString &message);
    void stateAdded(const GammaRay::StateId stateId, const GammaRay::StateId parentId,
                    const bool hasChildren, const QString &label, const GammaRay::StateType type,
                    const bool connectToInitial);
    void stateConfigurationChanged(const GammaRay::StateMachineConfiguration &config);
    void transitionAdded(const GammaRay::TransitionId transitionId,
                         const GammaRay::StateId sourceId,
                         const GammaRay::StateId targetId, const QString &label);
    void statusChanged(const bool haveStateMachine, const bool running);
    void transitionTriggered(GammaRay::TransitionId transitionId, const QString &label);
    void stateModelReset();

    void repopulateView();
    void clearGraph();

    void setShowLog(bool show);

    void objectInspectorContextMenu(QPoint pos);

private:
    /**
     * Show context menu for index @p index (a object inspector model index)
     * at global position @p globalPos.
     */
    void showContextMenuForObject(const QModelIndex &index, const QPoint &globalPos);

    void loadSettings();
    void saveSettings();

    QScopedPointer<Ui::StateMachineViewerWidget> m_ui;
    UIStateManager m_stateManager;

    KDSME::StateMachineView *m_stateMachineView;
    StateMachineViewerInterface *m_interface;

    QHash<StateId, KDSME::State *> m_idToStateMap;
    QHash<TransitionId, KDSME::Transition *> m_idToTransitionMap;
    KDSME::StateMachine *m_machine;
    bool m_showLog;
};

class StateMachineViewerUiFactory : public QObject,
                                    public StandardToolUiFactory<StateMachineViewerWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_statemachineviewer.json")
};
}

#endif // STATEMACHINEVIEWERWIDGETNG_H
