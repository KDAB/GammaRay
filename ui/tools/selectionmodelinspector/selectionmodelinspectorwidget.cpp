/*
  selectionmodelinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "selectionmodelinspectorwidget.h"
#include "ui_selectionmodelinspectorwidget.h"

#include <include/objectmodel.h>
#include <network/objectbroker.h>

using namespace GammaRay;

SelectionModelInspectorWidget::SelectionModelInspectorWidget(QWidget *widget)
  : QWidget(widget), ui(new Ui::SelectionModelInspectorWidget)
{
  ui->setupUi(this);

  ui->selectionModelView->setModel(ObjectBroker::model("com.kdab.GammaRay.SelectionModelsModel"));
  ui->selectionModelView->setRootIsDecorated(false);
  ui->selectionModelVisualizer->setRootIsDecorated(false);
  connect(ui->selectionModelView->selectionModel(),
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(selectionModelSelected(QItemSelection,QItemSelection)));
}

SelectionModelInspectorWidget::~SelectionModelInspectorWidget()
{
}

void SelectionModelInspectorWidget::selectionModelSelected(const QItemSelection &selected,
                                                     const QItemSelection &deselected)
{
  Q_UNUSED(deselected);
  QModelIndex selectedRow;
  if (!selected.isEmpty())
    selectedRow = selected.first().topLeft();
  QObject *selectionModelObject = selectedRow.data(ObjectModel::ObjectRole).value<QObject*>();
  QItemSelectionModel *selectionModel = qobject_cast<QItemSelectionModel*>(selectionModelObject);
  if (selectionModel && selectionModel->model()) {
    ui->selectionModelVisualizer->setModel(
      const_cast<QAbstractItemModel*>(selectionModel->model()));
    ui->selectionModelVisualizer->setSelectionModel(selectionModel);
  }
}

#include "selectionmodelinspectorwidget.moc"
