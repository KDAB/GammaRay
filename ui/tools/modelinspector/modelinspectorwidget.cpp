/*
  modelinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "modelinspectorwidget.h"
#include "ui_modelinspectorwidget.h"
#include "modelinspectorclient.h"

#include <ui/deferredresizemodesetter.h>
#include <ui/searchlinecontroller.h>
#include <common/endpoint.h>
#include <common/objectbroker.h>
#include <common/objectmodel.h>

#include <QDebug>

using namespace GammaRay;

static QObject* createModelInspectorClient(const QString & /*name*/, QObject *parent)
{
  return new ModelInspectorClient(parent);
}

ModelInspectorWidget::ModelInspectorWidget(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::ModelInspectorWidget)
  , m_interface(0)
{
  ui->setupUi(this);

  ObjectBroker::registerClientObjectFactoryCallback<ModelInspectorInterface*>(createModelInspectorClient);
  m_interface = ObjectBroker::object<ModelInspectorInterface*>();
  connect(m_interface, SIGNAL(cellSelected(int,int,QString,QString)),
          SLOT(cellSelected(int,int,QString,QString)));

  auto modelModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ModelModel"));
  ui->modelView->setModel(modelModel);
  ui->modelView->setSelectionModel(ObjectBroker::selectionModel(modelModel));
  new SearchLineController(ui->modelSearchLine, modelModel);
  connect(ui->modelView->selectionModel(),
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(modelSelected(QItemSelection)));
  new DeferredResizeModeSetter(ui->modelView->header(), 0, QHeaderView::ResizeToContents);

  ui->modelCellView->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ModelCellModel")));

  cellSelected(-1, -1, QString(), QString());
}

ModelInspectorWidget::~ModelInspectorWidget()
{
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
      model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ModelContent"));
      ui->modelContentView->setModel(model);
      setupModelContentSelectionModel();
    }

    // in case selection is not directly triggered by the user
    ui->modelView->scrollTo(index, QAbstractItemView::EnsureVisible);
  } else {
    ui->modelContentView->setModel(0);
  }

  // clear the cell info box
  cellSelected(-1, -1, QString(), QString());
}

void ModelInspectorWidget::cellSelected(int row, int column, const QString &internalId, const QString &internalPtr)
{
  ui->indexLabel->setText(row != -1 ?
    tr("Row: %1 Column: %2").arg(row).arg(column) :
    tr("Invalid"));
  ui->internalIdLabel->setText(internalId);
  ui->internalPtrLabel->setText(internalPtr);
}

void ModelInspectorWidget::objectRegistered(const QString& objectName)
{
  if (objectName == QLatin1String("com.kdab.GammaRay.ModelContent.selection"))
    // delay, since it's not registered yet when the signal is emitted
    QMetaObject::invokeMethod(this, "setupModelContentSelectionModel", Qt::QueuedConnection);
}

void ModelInspectorWidget::setupModelContentSelectionModel()
{
  if (!ui->modelContentView->model())
    return;

  ui->modelContentView->setSelectionModel(ObjectBroker::selectionModel(ui->modelContentView->model()));
}

