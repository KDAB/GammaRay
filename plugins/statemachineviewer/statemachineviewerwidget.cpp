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

#include "statemachineviewerwidget.h"
#include "ui_statemachineviewerwidget.h"

#include "statemachineviewerclient.h"
#include "statemodeldelegate.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/probecontrollerinterface.h>
#include <ui/contextmenuextension.h>

#include <kdstatemachineeditor/core/elementmodel.h>
#include <kdstatemachineeditor/core/state.h>
#include <kdstatemachineeditor/core/transition.h>
#include <kdstatemachineeditor/core/runtimecontroller.h>
#include <kdstatemachineeditor/view/statemachinescene.h>
#include <kdstatemachineeditor/view/statemachinetoolbar.h>
#include <kdstatemachineeditor/view/statemachineview.h>

#include <QDebug>
#include <QMenu>
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
    SelectionModelSyncer(StateMachineViewerWidget *widget);

private Q_SLOTS:
    void handle_objectInspector_currentChanged(const QModelIndex &index);
    void handle_stateMachineView_currentChanged(const QModelIndex &index);

    StateMachineViewerWidget *m_widget;
    bool m_updatesEnabled;
};

SelectionModelSyncer::SelectionModelSyncer(StateMachineViewerWidget *widget)
    : QObject(widget)
    , m_widget(widget)
    , m_updatesEnabled(true)
{
    connect(widget->objectInspector()->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &SelectionModelSyncer::handle_objectInspector_currentChanged);
    connect(
        widget->stateMachineView()->scene()->selectionModel(), &QItemSelectionModel::currentChanged,
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
                                      Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
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
                                      Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    auto selectionModel = m_widget->objectInspector()->selectionModel();
    selectionModel->setCurrentIndex(matches.value(
                                        0),
                                    QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows);
}

QObject *createStateMachineViewerClient(const QString & /*name*/, QObject *parent)
{
    return new StateMachineViewerClient(parent);
}

KDSME::RuntimeController::Configuration toSmeConfiguration(const StateMachineConfiguration &config,
                                                           const QHash<StateId,
                                                                       KDSME::State *> &map)
{
    KDSME::RuntimeController::Configuration result;
    foreach (const StateId &id, config) {
        if (auto state = map.value(id))
            result << state;
    }
    return result;
}
}

StateMachineViewerWidget::StateMachineViewerWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_ui(new Ui::StateMachineViewerWidget)
    , m_stateManager(this)
    , m_machine(0)
    , m_showLog(false)
{
    ObjectBroker::registerClientObjectFactoryCallback<StateMachineViewerInterface *>(
        createStateMachineViewerClient);
    m_interface = ObjectBroker::object<StateMachineViewerInterface *>();

    m_ui->setupUi(this);

    // set up log expanding widget
    connect(m_ui->hideLogPushButton, &QPushButton::clicked, this, [this]() {
        setShowLog(false);
    });
    connect(m_ui->showLogPushButton, &QPushButton::clicked, this, [this]() {
        setShowLog(true);
    });
    setShowLog(false);

    QAbstractItemModel *stateMachineModel
        = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.StateMachineModel"));
    m_ui->stateMachinesView->setModel(stateMachineModel);
    connect(m_ui->stateMachinesView, SIGNAL(currentIndexChanged(int)), m_interface,
            SLOT(selectStateMachine(int)));

    QAbstractItemModel *stateModel
        = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.StateModel"));
    connect(stateModel, SIGNAL(modelReset()), this, SLOT(stateModelReset()));

    m_ui->singleStateMachineView->header()->setObjectName("singleStateMachineViewHeader");
    m_ui->singleStateMachineView->setExpandNewContent(true);
    m_ui->singleStateMachineView->setDeferredResizeMode(0, QHeaderView::Stretch);
    m_ui->singleStateMachineView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    m_ui->singleStateMachineView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_ui->singleStateMachineView->setItemDelegate(new StateModelDelegate(this));
    m_ui->singleStateMachineView->setModel(stateModel);
    connect(m_ui->singleStateMachineView, &QWidget::customContextMenuRequested, this,
            &StateMachineViewerWidget::objectInspectorContextMenu);

    connect(m_ui->actionStartStopStateMachine, SIGNAL(triggered()), m_interface,
            SLOT(toggleRunning()));
    addAction(m_ui->actionStartStopStateMachine);

    auto separatorAction = new QAction(this);
    separatorAction->setSeparator(true);
    addAction(separatorAction);

    m_stateMachineView = new KDSME::StateMachineView;
    m_ui->horizontalSplitter->addWidget(m_stateMachineView);

    m_stateMachineView->scene()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_stateMachineView->scene(), &KDSME::StateMachineScene::customContextMenuEvent,
            this, [this](KDSME::AbstractSceneContextMenuEvent *event) {
        const auto objectId
            = ObjectId(reinterpret_cast<QObject *>(event->elementUnderCursor()->internalId()));
        const auto model = objectInspector()->model();
        const auto matches = model->match(
            model->index(0, 0), ObjectModel::ObjectIdRole,
            QVariant::fromValue(objectId), 1,
            Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
        showContextMenuForObject(matches.value(0), event->globalPos());
    });

    connect(m_interface, SIGNAL(message(QString)), this, SLOT(showMessage(QString)));
    connect(m_interface, SIGNAL(stateConfigurationChanged(GammaRay::StateMachineConfiguration)),
            this, SLOT(stateConfigurationChanged(GammaRay::StateMachineConfiguration)));
    connect(m_interface,
            SIGNAL(stateAdded(GammaRay::StateId,GammaRay::StateId,bool,QString,GammaRay::StateType,bool)),
            this,
            SLOT(stateAdded(GammaRay::StateId,GammaRay::StateId,bool,QString,GammaRay::StateType,bool)));
    connect(m_interface,
            SIGNAL(transitionAdded(GammaRay::TransitionId,GammaRay::StateId,GammaRay::StateId,QString)),
            this,
            SLOT(transitionAdded(GammaRay::TransitionId,GammaRay::StateId,GammaRay::StateId,QString)));
    connect(m_interface, SIGNAL(statusChanged(bool,bool)), this, SLOT(statusChanged(bool,bool)));
    connect(m_interface, SIGNAL(transitionTriggered(GammaRay::TransitionId,QString)),
            this, SLOT(transitionTriggered(GammaRay::TransitionId,QString)));

    connect(m_interface, SIGNAL(aboutToRepopulateGraph()), this, SLOT(clearGraph()));
    connect(m_interface, SIGNAL(graphRepopulated()), this, SLOT(repopulateView()));

    // append actions for the state machine view
    KDSME::StateMachineToolBar *toolBar = new KDSME::StateMachineToolBar(m_stateMachineView, this);
    toolBar->setHidden(true);
    addActions(toolBar->actions());

    m_interface->repopulateGraph();

    // share selection model
    new SelectionModelSyncer(this);

    m_stateManager.setDefaultSizes(m_ui->verticalSplitter, UISizeVector() << "50%" << "50%");
    m_stateManager.setDefaultSizes(m_ui->horizontalSplitter, UISizeVector() << "30%" << "70%");

    loadSettings();
}

StateMachineViewerWidget::~StateMachineViewerWidget()
{
    saveSettings();
}

void StateMachineViewerWidget::saveTargetState(QSettings *settings) const
{
    settings->setValue("ShowLog", m_showLog);
    // TODO: settings->setValue("ShowTransitionsLabel", m_stateMachineView->scene()->isTransitionsLabelVisible());
    settings->setValue("MaximumDepth", m_stateMachineView->scene()->maximumDepth());
}

void StateMachineViewerWidget::restoreTargetState(QSettings *settings)
{
    setShowLog(settings->value("ShowLog", m_showLog).toBool());
    // TODO: m_stateMachineView->scene()->setTransitionsLabelVisible(settings->value("ShowTransitionsLabel", true).toBool());
    m_stateMachineView->scene()->setMaximumDepth(settings->value("MaximumDepth", 3).toInt());
}

void StateMachineViewerWidget::showContextMenuForObject(const QModelIndex &index,
                                                        const QPoint &globalPos)
{
    if (!index.isValid())
        return;

    Q_ASSERT(index.model() == objectInspector()->model());

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();

    QMenu menu(tr("Entity @ %1").arg(QLatin1String("0x") + QString::number(objectId.id(), 16)));
    ContextMenuExtension ext(objectId);
    ext.setLocation(ContextMenuExtension::Creation, index.data(
                        ObjectModel::CreationLocationRole).value<SourceLocation>());
    ext.setLocation(ContextMenuExtension::Declaration,
                    index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&menu);

    menu.exec(globalPos);
}

KDSME::StateMachineView *StateMachineViewerWidget::stateMachineView() const
{
    return m_stateMachineView;
}

DeferredTreeView *StateMachineViewerWidget::objectInspector() const
{
    return m_ui->singleStateMachineView;
}

void StateMachineViewerWidget::loadSettings()
{
    QSettings settings;
    settings.beginGroup("Plugin_StateMachineViewer");
    m_stateMachineView->setThemeName(settings.value("ThemeName", "SystemTheme").toString());
    settings.endGroup();
    settings.sync();
}

void StateMachineViewerWidget::saveSettings()
{
    QSettings settings;
    settings.beginGroup("Plugin_StateMachineViewer");
    settings.setValue("ThemeName", m_stateMachineView->themeName());
    settings.endGroup();
    settings.sync();
}

void StateMachineViewerWidget::showMessage(const QString &message)
{
    // update log
    auto logTextEdit = m_ui->logTextEdit;
    logTextEdit->appendPlainText(message);

    // auto-scroll hack
    QScrollBar *sb = logTextEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void StateMachineViewerWidget::stateConfigurationChanged(const StateMachineConfiguration &config)
{
    if (m_machine)
        m_machine->runtimeController()->setActiveConfiguration(toSmeConfiguration(config,
                                                                                  m_idToStateMap));
}

void StateMachineViewerWidget::stateAdded(const StateId stateId, const StateId parentId,
                                          const bool hasChildren, const QString &label,
                                          const StateType type, const bool connectToInitial)
{
    Q_UNUSED(hasChildren);
    IF_DEBUG(qDebug() << "stateAdded" << stateId << parentId << label << type);

    if (m_idToStateMap.contains(stateId))
        return;

    KDSME::State *parentState = m_idToStateMap.value(parentId);
    KDSME::State *state = 0;
    if (type == StateMachineState)
        state = m_machine = new KDSME::StateMachine;
    else if (type == GammaRay::FinalState)
        state = new KDSME::FinalState(parentState);
    else if (type == GammaRay::ShallowHistoryState)
        state = new KDSME::HistoryState(KDSME::HistoryState::ShallowHistory, parentState);
    else if (type == GammaRay::DeepHistoryState)
        state = new KDSME::HistoryState(KDSME::HistoryState::DeepHistory, parentState);
    else
        state = new KDSME::State(parentState);

    if (connectToInitial && parentState) {
        KDSME::State *initialState = new KDSME::PseudoState(KDSME::PseudoState::InitialState,
                                                            parentState);
        initialState->setFlags(KDSME::Element::ElementIsSelectable);
        KDSME::Transition *transition = new KDSME::Transition(initialState);
        transition->setTargetState(state);
        transition->setFlags(KDSME::Element::ElementIsSelectable);
    }

    Q_ASSERT(state);
    state->setLabel(label);
    state->setInternalId(stateId);
    state->setFlags(KDSME::Element::ElementIsSelectable);
    m_idToStateMap[stateId] = state;
}

void StateMachineViewerWidget::transitionAdded(const TransitionId transitionId,
                                               const StateId sourceId, const StateId targetId,
                                               const QString &label)
{
    if (m_idToTransitionMap.contains(transitionId))
        return;

    IF_DEBUG(qDebug() << "transitionAdded" << transitionId << label << sourceId << targetId);

    KDSME::State *source = m_idToStateMap.value(sourceId);
    KDSME::State *target = m_idToStateMap.value(targetId);
    if (!source || !target) {
        qDebug() << "Null source or target for transition:" <<  transitionId;
        return;
    }

    KDSME::Transition *transition = new KDSME::Transition(source);
    transition->setTargetState(target);
    transition->setLabel(label);
    transition->setFlags(KDSME::Element::ElementIsSelectable);
    m_idToTransitionMap[transitionId] = transition;
}

void StateMachineViewerWidget::statusChanged(const bool haveStateMachine, const bool running)
{
    if (m_machine)
        m_machine->runtimeController()->setIsRunning(running);

    m_ui->actionStartStopStateMachine->setEnabled(haveStateMachine);
    if (!running) {
        m_ui->actionStartStopStateMachine->setText(tr("Start State Machine"));
        m_ui->actionStartStopStateMachine->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    } else {
        m_ui->actionStartStopStateMachine->setText(tr("Stop State Machine"));
        m_ui->actionStartStopStateMachine->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    }
}

void StateMachineViewerWidget::transitionTriggered(TransitionId transitionId, const QString &label)
{
    Q_UNUSED(label);
    if (m_machine)
        m_machine->runtimeController()->setLastTransition(m_idToTransitionMap.value(transitionId));
}

void StateMachineViewerWidget::clearGraph()
{
    IF_DEBUG(qDebug() << Q_FUNC_INFO);

    m_stateMachineView->scene()->setRootState(0);

    m_idToStateMap.clear();
    m_idToTransitionMap.clear();
}

void StateMachineViewerWidget::repopulateView()
{
    IF_DEBUG(qDebug() << Q_FUNC_INFO);

    m_stateMachineView->scene()->setRootState(m_machine);
    if (!m_machine)
        return;
    m_stateMachineView->scene()->layout();

    IF_DEBUG(m_machine->dumpObjectTree();)

    m_stateMachineView->fitInView();
}

void StateMachineViewerWidget::stateModelReset()
{
    m_ui->singleStateMachineView->expandAll();
    if (m_machine)
        m_machine->runtimeController()->clear();
}

void StateMachineViewerWidget::objectInspectorContextMenu(QPoint pos)
{
    const auto index = m_ui->singleStateMachineView->indexAt(pos);
    if (!index.isValid())
        return;

    const auto globalPos = m_ui->singleStateMachineView->viewport()->mapToGlobal(pos);
    showContextMenuForObject(index, globalPos);
}

void StateMachineViewerWidget::setShowLog(bool show)
{
    m_showLog = show;
    m_ui->logExpandingWidget->setVisible(show);
    m_ui->showLogPushButton->setVisible(!show);
    m_ui->verticalSplitter->handle(0)->setEnabled(show);
}
