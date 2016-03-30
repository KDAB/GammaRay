/*
  statemachineviewerwidgetng.cpp

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

#include "statemachineviewerwidgetng.h"
#include "ui_statemachineviewer.h"

#include "statemachineviewerclient.h"
#include "statemodeldelegate.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/probecontrollerinterface.h>
#include <ui/deferredresizemodesetter.h>
#include <ui/deferredtreeviewconfiguration.h>

#include <kdstatemachineeditor/core/elementmodel.h>
#include <kdstatemachineeditor/core/state.h>
#include <kdstatemachineeditor/core/transition.h>
#include <kdstatemachineeditor/core/runtimecontroller.h>
#include <kdstatemachineeditor/view/statemachinescene.h>
#include <kdstatemachineeditor/view/statemachinetoolbar.h>
#include <kdstatemachineeditor/view/statemachineview.h>

#include <QDebug>
#include <QLayout>
#include <QScopedValueRollback>
#include <QScrollBar>
#include <QSettings>

#define IF_DEBUG(x)

using namespace GammaRay;

namespace {

class SelectionModelSyncer : public QObject
{
public:
  SelectionModelSyncer(StateMachineViewerWidgetNG *widget);

private Q_SLOTS:
  void handle_objectInspector_currentChanged(const QModelIndex &index);
  void handle_stateMachineView_currentChanged(const QModelIndex &index);

  StateMachineViewerWidgetNG *m_widget;
  bool m_updatesEnabled;
};

SelectionModelSyncer::SelectionModelSyncer(StateMachineViewerWidgetNG *widget)
  : QObject(widget)
  , m_widget(widget)
  , m_updatesEnabled(true)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
  QMetaType::registerComparators<ObjectId>(); // for proper QAIM::match support for ObjectId
#endif

  connect(widget->objectInspector()->selectionModel(), &QItemSelectionModel::currentChanged,
          this, &SelectionModelSyncer::handle_objectInspector_currentChanged);
  connect(widget->stateMachineView()->scene()->selectionModel(), &QItemSelectionModel::currentChanged,
          this, &SelectionModelSyncer::handle_stateMachineView_currentChanged);
}

void SelectionModelSyncer::handle_objectInspector_currentChanged(const QModelIndex &index)
{
  if (!m_updatesEnabled)
    return;

  QScopedValueRollback<bool> block(m_updatesEnabled);
  m_updatesEnabled = false;

  const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
  const auto model = m_widget->stateMachineView()->scene()->model();
  const auto matches = model->match(model->index(0, 0), KDSME::StateModel::InternalIdRole,
                                    static_cast<quintptr>(objectId.id()), 1,
                                    Qt::MatchExactly | Qt::MatchRecursive);
  auto selectionModel = m_widget->stateMachineView()->scene()->selectionModel();
  selectionModel->setCurrentIndex(matches.value(0), QItemSelectionModel::SelectCurrent);
}

void SelectionModelSyncer::handle_stateMachineView_currentChanged(const QModelIndex &index)
{
  if (!m_updatesEnabled)
    return;

  QScopedValueRollback<bool> block(m_updatesEnabled);
  m_updatesEnabled = false;

  const auto internalId = index.data(KDSME::StateModel::InternalIdRole).value<quintptr>();
  const auto objectId = ObjectId(reinterpret_cast<QObject *>(internalId));
  const auto model = m_widget->objectInspector()->model();
  const auto matches = model->match(model->index(0, 0), ObjectModel::ObjectIdRole,
                                    QVariant::fromValue(objectId), 1,
                                    Qt::MatchExactly | Qt::MatchRecursive);
  auto selectionModel = m_widget->objectInspector()->selectionModel();
  selectionModel->setCurrentIndex(matches.value(0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

QObject* createStateMachineViewerClient(const QString &/*name*/, QObject *parent)
{
  return new StateMachineViewerClient(parent);
}

KDSME::RuntimeController::Configuration toSmeConfiguration(const StateMachineConfiguration& config,
                                                                 const QHash<StateId, KDSME::State*>& map)
{
  KDSME::RuntimeController::Configuration result;
  foreach (const StateId& id, config) {
    if (auto state = map.value(id)) {
      result << state;
    }
  }
  return result;
}

}

StateMachineViewerWidgetNG::StateMachineViewerWidgetNG(QWidget* parent, Qt::WindowFlags f)
  : QWidget(parent, f)
  , m_ui(new Ui::StateMachineViewer)
  , m_machine(0)
{
  ObjectBroker::registerClientObjectFactoryCallback<StateMachineViewerInterface*>(createStateMachineViewerClient);
  m_interface = ObjectBroker::object<StateMachineViewerInterface*>();

  m_ui->setupUi(this);

  // set up log expanding widget
  connect(m_ui->hideLogPushButton, &QPushButton::clicked, this, [this]() { setShowLog(false); });
  connect(m_ui->showLogPushButton, &QPushButton::clicked, this, [this]() { setShowLog(true); });
  setShowLog(false);

  QAbstractItemModel *stateMachineModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.StateMachineModel"));
  m_ui->stateMachinesView->setModel(stateMachineModel);
  connect(m_ui->stateMachinesView, SIGNAL(currentIndexChanged(int)), m_interface, SLOT(selectStateMachine(int)));

  QAbstractItemModel *stateModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.StateModel"));
  connect(stateModel, SIGNAL(modelReset()), this, SLOT(stateModelReset()));

  m_ui->singleStateMachineView->setModel(stateModel);
  m_ui->singleStateMachineView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  new DeferredResizeModeSetter(m_ui->singleStateMachineView->header(), 0, QHeaderView::Stretch);
  new DeferredResizeModeSetter(m_ui->singleStateMachineView->header(), 1, QHeaderView::ResizeToContents);
  new DeferredTreeViewConfiguration(m_ui->singleStateMachineView, true);
  m_ui->singleStateMachineView->setItemDelegate(new StateModelDelegate(this));

  connect(m_ui->actionStartStopStateMachine, SIGNAL(triggered()), m_interface, SLOT(toggleRunning()));
  addAction(m_ui->actionStartStopStateMachine);

  auto separatorAction = new QAction(this);
  separatorAction->setSeparator(true);
  addAction(separatorAction);

  m_stateMachineView = new KDSME::StateMachineView;
  m_ui->horizontalSplitter->setChildrenCollapsible(false);
  m_ui->horizontalSplitter->addWidget(m_stateMachineView);
  m_ui->horizontalSplitter->setStretchFactor(m_ui->horizontalSplitter->indexOf(m_stateMachineView), 3);

  connect(m_interface, SIGNAL(message(QString)), this, SLOT(showMessage(QString)));
  connect(m_interface, SIGNAL(stateConfigurationChanged(GammaRay::StateMachineConfiguration)),
          this, SLOT(stateConfigurationChanged(GammaRay::StateMachineConfiguration)));
  connect(m_interface, SIGNAL(stateAdded(GammaRay::StateId,GammaRay::StateId,bool,QString,GammaRay::StateType,bool)),
          this, SLOT(stateAdded(GammaRay::StateId,GammaRay::StateId,bool,QString,GammaRay::StateType,bool)));
  connect(m_interface, SIGNAL(transitionAdded(GammaRay::TransitionId,GammaRay::StateId,GammaRay::StateId,QString)),
          this, SLOT(transitionAdded(GammaRay::TransitionId,GammaRay::StateId,GammaRay::StateId,QString)));
  connect(m_interface, SIGNAL(statusChanged(bool,bool)), this, SLOT(statusChanged(bool,bool)));
  connect(m_interface, SIGNAL(transitionTriggered(GammaRay::TransitionId,QString)),
          this, SLOT(transitionTriggered(GammaRay::TransitionId,QString)));

  connect(m_interface, SIGNAL(aboutToRepopulateGraph()), this, SLOT(clearGraph()));
  connect(m_interface, SIGNAL(graphRepopulated()), this, SLOT(repopulateView()));

  // append actions for the state machine view
  KDSME::StateMachineToolBar* toolBar = new KDSME::StateMachineToolBar(m_stateMachineView, this);
  toolBar->setHidden(true);
  addActions(toolBar->actions());

  m_interface->repopulateGraph();

  // share selection model
  new SelectionModelSyncer(this);

  loadSettings();
}

StateMachineViewerWidgetNG::~StateMachineViewerWidgetNG()
{
  saveSettings();
}

KDSME::StateMachineView *StateMachineViewerWidgetNG::stateMachineView() const
{
  return m_stateMachineView;
}

QTreeView *StateMachineViewerWidgetNG::objectInspector() const
{
  return m_ui->singleStateMachineView;
}


void StateMachineViewerWidgetNG::loadSettings()
{
  QSettings settings;
  settings.beginGroup("Plugin_StateMachineViewer");
  m_stateMachineView->setThemeName(settings.value("ThemeName", "SystemTheme").toString());
  settings.endGroup();
  settings.sync();
}

void StateMachineViewerWidgetNG::saveSettings()
{
  QSettings settings;
  settings.beginGroup("Plugin_StateMachineViewer");
  settings.setValue("ThemeName", m_stateMachineView->themeName());
  settings.endGroup();
  settings.sync();
}

void StateMachineViewerWidgetNG::showMessage(const QString& message)
{
    // update log
  auto logTextEdit = m_ui->logTextEdit;
  logTextEdit->appendPlainText(message);

  // auto-scroll hack
  QScrollBar *sb = logTextEdit->verticalScrollBar();
  sb->setValue(sb->maximum());
}

void StateMachineViewerWidgetNG::stateConfigurationChanged(const StateMachineConfiguration& config)
{
  if (m_machine) {
    m_machine->runtimeController()->setActiveConfiguration(toSmeConfiguration(config, m_idToStateMap));
  }
}

void StateMachineViewerWidgetNG::stateAdded(const StateId stateId, const StateId parentId, const bool hasChildren,
                                            const QString& label, const StateType type, const bool connectToInitial)
{
  Q_UNUSED(hasChildren);
  IF_DEBUG(qDebug() << "stateAdded" << stateId << parentId << label << type);

  if (m_idToStateMap.contains(stateId)) {
    return;
  }

  KDSME::State* parentState = m_idToStateMap.value(parentId);
  KDSME::State* state = 0;
  if (type == StateMachineState) {
    state = m_machine = new KDSME::StateMachine;
  } else if (type == GammaRay::FinalState) {
    state = new KDSME::FinalState(parentState);
  } else if (type == GammaRay::ShallowHistoryState) {
    state = new KDSME::HistoryState(KDSME::HistoryState::ShallowHistory, parentState);
  } else if (type == GammaRay::DeepHistoryState) {
    state = new KDSME::HistoryState(KDSME::HistoryState::DeepHistory, parentState);
  } else {
    state = new KDSME::State(parentState);
  }

  if (connectToInitial && parentState) {
    KDSME::State* initialState = new KDSME::PseudoState(KDSME::PseudoState::InitialState, parentState);
    initialState->setFlags(KDSME::Element::ElementIsSelectable);
    KDSME::Transition* transition = new KDSME::Transition(initialState);
    transition->setTargetState(state);
    transition->setFlags(KDSME::Element::ElementIsSelectable);
  }

  Q_ASSERT(state);
  state->setLabel(label);
  state->setInternalId(stateId);
  state->setFlags(KDSME::Element::ElementIsSelectable);
  m_idToStateMap[stateId] = state;
}

void StateMachineViewerWidgetNG::transitionAdded(const TransitionId transitionId, const StateId sourceId, const StateId targetId, const QString& label)
{
  if (m_idToTransitionMap.contains(transitionId))
    return;

  IF_DEBUG(qDebug() << "transitionAdded" << transitionId << label << sourceId << targetId);

  KDSME::State* source = m_idToStateMap.value(sourceId);
  KDSME::State* target = m_idToStateMap.value(targetId);
  if (!source || !target) {
    qDebug() << "Null source or target for transition:" <<  transitionId;
    return;
  }

  KDSME::Transition* transition = new KDSME::Transition(source);
  transition->setTargetState(target);
  transition->setLabel(label);
  transition->setFlags(KDSME::Element::ElementIsSelectable);
  m_idToTransitionMap[transitionId] = transition;
}

void StateMachineViewerWidgetNG::statusChanged(const bool haveStateMachine, const bool running)
{
  if (m_machine) {
    m_machine->runtimeController()->setIsRunning(running);
  }

  m_ui->actionStartStopStateMachine->setEnabled(haveStateMachine);
  if (!running) {
    m_ui->actionStartStopStateMachine->setText(tr("Start State Machine"));
    m_ui->actionStartStopStateMachine->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  } else {
    m_ui->actionStartStopStateMachine->setText(tr("Stop State Machine"));
    m_ui->actionStartStopStateMachine->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
  }
}

void StateMachineViewerWidgetNG::transitionTriggered(TransitionId transitionId, const QString& label)
{
  Q_UNUSED(label);
  if (m_machine) {
    m_machine->runtimeController()->setLastTransition(m_idToTransitionMap.value(transitionId));
  }
}

void StateMachineViewerWidgetNG::clearGraph()
{
  IF_DEBUG(qDebug() << Q_FUNC_INFO);

  m_stateMachineView->scene()->setRootState(0);

  m_idToStateMap.clear();
  m_idToTransitionMap.clear();
}

void StateMachineViewerWidgetNG::repopulateView()
{
  IF_DEBUG(qDebug() << Q_FUNC_INFO);

  m_stateMachineView->scene()->setRootState(m_machine);
  if (!m_machine)
    return;
  m_stateMachineView->scene()->layout();

  IF_DEBUG(m_machine->dumpObjectTree();)

  m_stateMachineView->fitInView();
}

void StateMachineViewerWidgetNG::stateModelReset()
{
  m_ui->singleStateMachineView->expandAll();
  if (m_machine) {
    m_machine->runtimeController()->clear();
  }
}

void StateMachineViewerWidgetNG::setShowLog(bool show)
{
  m_ui->logExpandingWidget->setVisible(show);
  m_ui->showLogPushButton->setVisible(!show);
  m_ui->verticalSplitter->handle(0)->setEnabled(show);
  m_ui->verticalSplitter->setSizes(QList<int>() << 1 << (show ? 1 : 0));
}
