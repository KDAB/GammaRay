/*
  modelinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "modelinspectorwidget.h"
#include "ui_modelinspectorwidget.h"

#include "include/objectmodel.h"
#include "include/util.h"

#include <network/endpoint.h>
#include <network/objectbroker.h>

#include <kde/krecursivefilterproxymodel.h>
#include <QDebug>

using namespace GammaRay;

ModelInspectorWidget::ModelInspectorWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::ModelInspectorWidget)
{
  ui->setupUi(this);

  KRecursiveFilterProxyModel *modelFilterProxy = new KRecursiveFilterProxyModel(this);
  modelFilterProxy->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.ModelModel"));
  ui->modelView->setModel(modelFilterProxy);
  ui->modelView->setSelectionModel(ObjectBroker::selectionModel(modelFilterProxy));
  ui->modelSearchLine->setProxy(modelFilterProxy);
  connect(ui->modelView->selectionModel(),
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(modelSelected(QItemSelection)));

  ui->modelCellView->setModel(ObjectBroker::model("com.kdab.GammaRay.ModelCellModel"));

  setModelCell(QModelIndex());
}

void ModelInspectorWidget::modelSelected(const QItemSelection& selected)
{
  QModelIndex index;
  if (selected.size() >= 1)
    index = selected.first().topLeft();

  if (index.isValid()) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    QAbstractItemModel *model = qobject_cast<QAbstractItemModel*>(obj);
    if (model) {
      // we are on the server side
      ui->modelContentView->setModel(model);
      if (ObjectBroker::hasSelectionModel(ui->modelContentView->model()))
        setupModelContentSelectionModel();
      connect(Endpoint::instance(), SIGNAL(objectRegistered(QString,Protocol::ObjectAddress)),
              this, SLOT(objectRegistered(QString)), Qt::UniqueConnection);
    } else {
      // we are on the client side
      model = ObjectBroker::model("com.kdab.GammaRay.ModelContent");
      ui->modelContentView->setModel(model);
      setupModelContentSelectionModel();
    }

    // in case selection is not directly triggered by the user
    ui->modelView->scrollTo(index, QAbstractItemView::EnsureVisible);
  } else {
    ui->modelContentView->setModel(0);
  }

  // clear the cell info box
  setModelCell(QModelIndex());
}

void ModelInspectorWidget::setModelCell(const QModelIndex &index)
{
  // TODO this wont work remotely!
  ui->indexLabel->setText(index.isValid() ?
    tr("Row: %1 Column: %2").arg(index.row()).arg(index.column()) :
    tr("Invalid"));
  ui->internalIdLabel->setText(QString::number(index.internalId()));
  ui->internalPtrLabel->setText(Util::addressToString(index.internalPointer()));
}

void ModelInspectorWidget::objectRegistered(const QString& objectName)
{
  if (objectName == "com.kdab.GammaRay.ModelContent.selection")
    // delay, since it's not registered yet when the signal is emitted
    QMetaObject::invokeMethod(this, "setupModelContentSelectionModel", Qt::QueuedConnection);
}

void ModelInspectorWidget::setupModelContentSelectionModel()
{
  if (!ui->modelContentView->model())
    return;

  ui->modelContentView->setSelectionModel(ObjectBroker::selectionModel(ui->modelContentView->model()));
  connect(ui->modelContentView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          this, SLOT(setModelCell(QModelIndex)), Qt::UniqueConnection);
}

#include "modelinspectorwidget.moc"
