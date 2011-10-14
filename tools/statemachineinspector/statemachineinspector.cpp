/*
  statemachineinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "statemachineinspector.h"
#include "ui_statemachineinspector.h"

#include "statemodel.h"
#include "transitionmodel.h"

#include <objecttypefilterproxymodel.h>
#include <probeinterface.h>

using namespace GammaRay;

StateMachineInspector::StateMachineInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent), ui(new Ui::StateMachineInspector)
{
  ui->setupUi(this);

  ObjectTypeFilterProxyModel<QStateMachine> *stateMachineFilter =
    new ObjectTypeFilterProxyModel<QStateMachine>(this);
  stateMachineFilter->setSourceModel(probe->objectListModel());
  ui->stateMachinesView->setModel(stateMachineFilter);
  connect(ui->stateMachinesView->selectionModel(),
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(stateMachineSelected(QItemSelection,QItemSelection)));
  m_stateModel = 0;

  m_transitionModel = new TransitionModel(this);
  ui->transitionView->setModel(m_transitionModel);
}

void StateMachineInspector::stateMachineSelected(const QItemSelection &selected,
                                                 const QItemSelection &deselected)
{
  Q_UNUSED(deselected)
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *machineObject = selectedRow.data(ObjectModel::ObjectRole).value<QObject*>();
  QStateMachine *machine = qobject_cast<QStateMachine*>(machineObject);
  if (machine) {
    delete m_stateModel;
    m_stateModel = new StateModel(machine, this);
    ui->singleStateMachineView->setModel(m_stateModel);
    ui->singleStateMachineView->expandAll();
    connect(ui->singleStateMachineView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(stateSelected(QItemSelection,QItemSelection)));
  }
}

void StateMachineInspector::stateSelected(const QItemSelection &selected,
                                          const QItemSelection &deselected)
{
  Q_UNUSED(deselected);
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *stateObject = selectedRow.data(StateModel::StateObjectRole).value<QObject*>();
  QState *state = qobject_cast<QState*>(stateObject);
  if (state) {
    m_transitionModel->setState(state);
  }
}

#include "statemachineinspector.moc"
