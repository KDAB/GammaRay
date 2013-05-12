/*
  sceneinspectorwidget.cpp

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

#include "sceneinspectorwidget.h"

#include "scenemodel.h"
#include "ui_sceneinspectorwidget.h"

#include <common/network/objectbroker.h>

#include "include/objectmodel.h"
#include "include/util.h"

#include <kde/krecursivefilterproxymodel.h>

#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsView>

using namespace GammaRay;

SceneInspectorWidget::SceneInspectorWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::SceneInspectorWidget)
{
  ui->setupUi(this);
  ui->scenePropertyWidget->setObjectBaseName("com.kdab.GammaRay.SceneInspector");

  ui->sceneComboBox->setModel(ObjectBroker::model("com.kdab.GammaRay.SceneList"));
  connect(ui->sceneComboBox, SIGNAL(activated(int)), SLOT(sceneSelected(int)));

  QSortFilterProxyModel *sceneFilter = new KRecursiveFilterProxyModel(this);
  sceneFilter->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.SceneGraphModel"));
  ui->sceneTreeView->setModel(sceneFilter);
  ui->screneTreeSearchLine->setProxy(sceneFilter);

  QItemSelectionModel *itemSelection = ObjectBroker::selectionModel(sceneFilter);
  ui->sceneTreeView->setSelectionModel(itemSelection);
  connect(itemSelection, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(sceneItemSelected(QItemSelection)));

  if (ui->sceneComboBox->count()) {
    sceneSelected(0);
  }
}

SceneInspectorWidget::~SceneInspectorWidget()
{
}

void SceneInspectorWidget::sceneSelected(int index)
{
  const QModelIndex mi = ui->sceneComboBox->model()->index(index, 0);
  ObjectBroker::selectionModel(ui->sceneComboBox->model())->select(mi, QItemSelectionModel::ClearAndSelect);

  QObject *obj = ui->sceneComboBox->itemData(index, ObjectModel::ObjectRole).value<QObject*>();
  QGraphicsScene *scene = qobject_cast<QGraphicsScene*>(obj);
  qDebug() << Q_FUNC_INFO << scene << obj;

  ui->graphicsSceneView->setGraphicsScene(scene);
}

void SceneInspectorWidget::sceneItemSelected(const QItemSelection &selection)
{
  QModelIndex index;
  if (!selection.isEmpty())
    index = selection.first().topLeft();

  if (index.isValid()) {
    QGraphicsItem *item = index.data(SceneModel::SceneItemRole).value<QGraphicsItem*>();
    ui->graphicsSceneView->showGraphicsItem(item);
    ui->sceneTreeView->scrollTo(index); // in case selection does not come from us
  }
}

#include "sceneinspectorwidget.moc"
