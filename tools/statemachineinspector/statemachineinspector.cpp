#include "statemachineinspector.h"
#include "ui_statemachineinspector.h"

#include "statemodel.h"
#include "transitionmodel.h"

#include <objecttypefilterproxymodel.h>
#include <probeinterface.h>

using namespace Endoscope;

StateMachineInspector::StateMachineInspector(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui( new Ui::StateMachineInspector )
{
  ui->setupUi( this );

  ObjectTypeFilterProxyModel<QStateMachine> *stateMachineFilter = new ObjectTypeFilterProxyModel<QStateMachine>( this );
  stateMachineFilter->setSourceModel( probe->objectListModel() );
  ui->stateMachinesView->setModel(stateMachineFilter);
  connect( ui->stateMachinesView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(stateMachineSelected(QItemSelection,QItemSelection)));
  m_stateModel = 0;

  m_transitionModel = new TransitionModel(this);
  ui->transitionView->setModel(m_transitionModel);


}

void StateMachineInspector::stateMachineSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
  Q_UNUSED(deselected)
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *machineObject = selectedRow.data( ObjectListModel::ObjectRole ).value<QObject*>();
  QStateMachine *machine = qobject_cast<QStateMachine*>(machineObject);
  if (machine) {
    delete m_stateModel;
    m_stateModel = new StateModel( machine, this );
    ui->singleStateMachineView->setModel(m_stateModel);
    ui->singleStateMachineView->expandAll();
    connect(ui->singleStateMachineView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                                                         SLOT(stateSelected(QItemSelection,QItemSelection)));
  }
}

void StateMachineInspector::stateSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
  Q_UNUSED(deselected);
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *stateObject = selectedRow.data( StateModel::StateObjectRole ).value<QObject*>();
  QState *state = qobject_cast<QState*>(stateObject);
  if (state) {
    m_transitionModel->setState(state);
  }
}

#include "statemachineinspector.moc"
