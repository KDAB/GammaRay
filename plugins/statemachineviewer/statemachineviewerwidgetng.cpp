#include "statemachineviewerwidgetng.h"
#include "ui_statemachineviewer.h"

#include "statemachineviewerclient.h"

#include <common/objectbroker.h>
#include <ui/deferredresizemodesetter.h>
#include <ui/deferredtreeviewconfiguration.h>

#include <sme/core/element.h>
#include <sme/core/layoutinformation.h>
#include <sme/core/view.h>
#include <sme/view/statemachineview.h>

#include <QDebug>
#include <QLayout>
#include <QScrollBar>
#include <QSettings>

#define IF_DEBUG(x)

using namespace GammaRay;
using namespace SME;

namespace {

QObject* createStateMachineViewerClient(const QString &/*name*/, QObject *parent)
{
  return new StateMachineViewerClient(parent);
}

ConfigurationWatcher::Configuration toSmeConfiguration(const StateMachineConfiguration& config, const QHash<StateId, State*>& map)
{
  ConfigurationWatcher::Configuration result;
  foreach (const StateId& id, config) {
    result << map[id];
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

  m_ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
  m_ui->graphicsView->setScene(new QGraphicsScene(this));
  m_ui->graphicsView->setRenderHint(QPainter::Antialiasing);

  QAbstractItemModel *stateMachineModel = ObjectBroker::model("com.kdab.GammaRay.StateMachineModel");
  m_ui->stateMachinesView->setModel(stateMachineModel);
  m_ui->stateMachinesView->setSelectionModel(ObjectBroker::selectionModel(stateMachineModel));
  new DeferredResizeModeSetter(m_ui->stateMachinesView->header(), 0, QHeaderView::Stretch);
  new DeferredResizeModeSetter(m_ui->stateMachinesView->header(), 1, QHeaderView::ResizeToContents);
  new DeferredTreeViewConfiguration(m_ui->stateMachinesView, false);

  QAbstractItemModel *stateModel = ObjectBroker::model("com.kdab.GammaRay.StateModel");
  connect(stateModel, SIGNAL(modelReset()), this, SLOT(stateModelReset()));

  m_ui->singleStateMachineView->setModel(stateModel);
  m_ui->singleStateMachineView->setSelectionModel(ObjectBroker::selectionModel(stateModel));
  m_ui->singleStateMachineView->setSelectionMode(QAbstractItemView::ExtendedSelection);
  new DeferredResizeModeSetter(m_ui->singleStateMachineView->header(), 0, QHeaderView::Stretch);
  new DeferredResizeModeSetter(m_ui->singleStateMachineView->header(), 1, QHeaderView::ResizeToContents);
  new DeferredTreeViewConfiguration(m_ui->singleStateMachineView, true, false);

  connect(m_ui->depthSpinBox, SIGNAL(valueChanged(int)), m_interface, SLOT(setMaximumDepth(int)));
  connect(m_ui->startStopButton, SIGNAL(clicked()), m_interface, SLOT(toggleRunning()));

  // TODO: Re-enable?
  //connect(m_ui->exportButton, SIGNAL(clicked()), SLOT(exportAsImage()));
  m_ui->exportButton->setDisabled(true);

  m_ui->maxMegaPixelsSpinBox->setValue(maximumMegaPixels());
  connect(m_ui->maxMegaPixelsSpinBox, SIGNAL(valueChanged(int)), SLOT(setMaximumMegaPixels(int)));

  m_currentView = new View(this);
  m_stateMachineView = new SME::StateMachineView;
  m_stateMachineView->setView(m_currentView);
  QWidget* container = QWidget::createWindowContainer(m_stateMachineView);
  container->setMinimumWidth(300);

  // FIXME: Do it properly
  delete m_ui->graphicsView;
  m_ui->verticalSplitter->addWidget(container);

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

  m_interface->repopulateGraph();
}

StateMachineViewerWidgetNG::~StateMachineViewerWidgetNG()
{
}

int StateMachineViewerWidgetNG::maximumMegaPixels() const
{
  return QSettings().value("StateMachineViewerServer/maximumMegaPixels", 10).toInt();
}

void StateMachineViewerWidgetNG::setMaximumMegaPixels(int megaPixels)
{
  QSettings().setValue("StateMachineViewerServer/maximumMegaPixels", megaPixels);
}

void StateMachineViewerWidgetNG::showMessage(const QString& message)
{
    // update log
  QPlainTextEdit *plainTextEdit = m_ui->plainTextEdit;
  plainTextEdit->appendPlainText(message);

  // auto-scroll hack
  QScrollBar *sb = plainTextEdit->verticalScrollBar();
  sb->setValue(sb->maximum());
}

void StateMachineViewerWidgetNG::stateConfigurationChanged(const StateMachineConfiguration& config)
{
  m_currentView->configurationWatcher()->setActiveConfiguration(toSmeConfiguration(config, m_idToStateMap));
}

void StateMachineViewerWidgetNG::stateAdded(const StateId stateId, const StateId parentId, const bool hasChildren,
                                            const QString& label, const StateType type, const bool connectToInitial)
{
  IF_DEBUG(qDebug() << "stateAdded" << stateId << parentId << label << type);

  if (m_idToStateMap.contains(stateId)) {
    return;
  }

  State* parentState = m_idToStateMap.value(parentId);
  State* state = 0;
  if (type == StateMachineState) {
    state = m_machine = new StateMachine;
  } else if (type == GammaRay::FinalState) {
    state = new SME::FinalState(parentState);
  } else if (hasChildren) {
    state = new CompositeState(parentState);
  } else {
    state = new State(parentState);
  }

  if (connectToInitial && parentState) {
    State* initialState = new PseudoState(PseudoState::InitialState, parentState);
    Transition* transition = new Transition(initialState);
    transition->setTargetState(state);
  }

  Q_ASSERT(state);
  state->setLabel(label);
  state->setInternalId(stateId);
  m_idToStateMap[stateId] = state;
}

void StateMachineViewerWidgetNG::transitionAdded(const TransitionId transitionId, const StateId sourceId, const StateId targetId, const QString& label)
{
  if (m_idToTransitionMap.contains(transitionId))
    return;

  IF_DEBUG(qDebug() << "transitionAdded" << transitionId << label << sourceId << targetId);

  State* source = m_idToStateMap.value(sourceId);
  State* target = m_idToStateMap.value(targetId);
  if (!source || !target) {
    qDebug() << "Null source or target for transition:" <<  transitionId;
    return;
  }

  Transition* transition = new Transition(source);
  transition->setTargetState(target);
  transition->setLabel(label);
  m_idToTransitionMap[transitionId] = transition;
}

void StateMachineViewerWidgetNG::statusChanged(const bool haveStateMachine, const bool running)
{
  m_currentView->configurationWatcher()->setIsRunning(running);

  if (!running) {
    m_ui->startStopButton->setChecked(false);
    m_ui->startStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
  } else {
    m_ui->startStopButton->setChecked(true);
    m_ui->startStopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
  }
  m_ui->startStopButton->setEnabled(haveStateMachine);
}

void StateMachineViewerWidgetNG::transitionTriggered(TransitionId transitionId, const QString& label)
{
  m_currentView->configurationWatcher()->setLastTransition(m_idToTransitionMap.value(transitionId));
}

void StateMachineViewerWidgetNG::clearGraph()
{
  IF_DEBUG(qDebug() << Q_FUNC_INFO);

  m_currentView->setStateMachine(0);

  m_idToStateMap.clear();
  m_idToTransitionMap.clear();
}

void StateMachineViewerWidgetNG::repopulateView()
{
  IF_DEBUG(qDebug() << Q_FUNC_INFO);

  m_currentView->setStateMachine(m_machine);

  IF_DEBUG(m_machine->dumpObjectTree();)

  m_stateMachineView->fitInView();
}

void StateMachineViewerWidgetNG::stateModelReset()
{
  m_ui->singleStateMachineView->expandAll();
  m_currentView->configurationWatcher()->clear();
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(StateMachineViewerUiFactory)
#endif
