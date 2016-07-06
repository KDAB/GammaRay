/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    explicit StateMachineViewerWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    virtual ~StateMachineViewerWidget();

    Q_INVOKABLE void saveTargetState(QSettings *settings) const;
    Q_INVOKABLE void restoreTargetState(QSettings *settings);

    KDSME::StateMachineView *stateMachineView() const;
    DeferredTreeView *objectInspector() const;

private slots:
    void showMessage(const QString &message);
    // cppcheck-suppress passedByValue
    void stateAdded(const GammaRay::StateId stateId, const GammaRay::StateId parentId,
                    const bool hasChildren, const QString &label, const GammaRay::StateType type,
                    const bool connectToInitial);
    void stateConfigurationChanged(const GammaRay::StateMachineConfiguration &config);
    // cppcheck-suppress passedByValue
    void transitionAdded(const GammaRay::TransitionId transitionId, const GammaRay::StateId source,
                         // cppcheck-suppress passedByValue
                         const GammaRay::StateId target, const QString &label);
    // cppcheck-suppress passedByValue
    void statusChanged(const bool haveStateMachine, const bool running);
    void transitionTriggered(GammaRay::TransitionId transition, const QString &label);
    void stateModelReset();

    void repopulateView();
    void clearGraph();

    void setShowLog(bool show);

    void objectInspectorContextMenu(QPoint pos);

private:
    /// Show context menu for index @p index (a object inspector model index) at pos @p pos
    void showContextMenuForObject(const QModelIndex &index, const QPoint &pos);

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
