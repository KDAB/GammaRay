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
#include "sceneinspectorclient.h"
#include "ui_sceneinspectorwidget.h"

#include <common/network/objectbroker.h>

#include "include/objectmodel.h"
#include "include/util.h"

#include <kde/krecursivefilterproxymodel.h>

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QDebug>

#include <iostream>

using namespace GammaRay;
using namespace std;

static QObject* createClientSceneInspector(const QString &/*name*/, QObject *parent)
{
  return new SceneInspectorClient(parent);
}

SceneInspectorWidget::SceneInspectorWidget(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::SceneInspectorWidget)
  , m_interface(0)
  , m_scene(new QGraphicsScene(this))
{
  ObjectBroker::registerClientObjectFactoryCallback<SceneInspectorInterface*>(createClientSceneInspector);
  m_interface = ObjectBroker::object<SceneInspectorInterface*>();

  ui->setupUi(this);
  ui->scenePropertyWidget->setObjectBaseName("com.kdab.GammaRay.SceneInspector");

  ui->sceneComboBox->setModel(ObjectBroker::model("com.kdab.GammaRay.SceneList"));
  connect(ui->sceneComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sceneSelected(int)));

  QSortFilterProxyModel *sceneFilter = new KRecursiveFilterProxyModel(this);
  sceneFilter->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.SceneGraphModel"));
  ui->sceneTreeView->setModel(sceneFilter);
  ui->screneTreeSearchLine->setProxy(sceneFilter);

  QItemSelectionModel *itemSelection = ObjectBroker::selectionModel(sceneFilter);
  ui->sceneTreeView->setSelectionModel(itemSelection);
  connect(itemSelection, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(sceneItemSelected(QItemSelection)));

  ui->graphicsSceneView->setGraphicsScene(m_scene);
  connect(m_interface, SIGNAL(sceneRectChanged(QRectF)),
          this, SLOT(sceneRectChanged(QRectF)));

  m_interface->initializeGui();

  QItemSelectionModel *selection = ObjectBroker::selectionModel(ui->sceneComboBox->model());
  if (selection->currentIndex().isValid()) {
    sceneSelected(selection->currentIndex().row());
  }
}

SceneInspectorWidget::~SceneInspectorWidget()
{
}

void SceneInspectorWidget::sceneRectChanged(const QRectF &rect)
{
  qDebug() << rect;
  m_scene->setSceneRect(rect);
}

void SceneInspectorWidget::sceneSelected(int index)
{
  const QModelIndex mi = ui->sceneComboBox->model()->index(index, 0);
  ObjectBroker::selectionModel(ui->sceneComboBox->model())->select(mi, QItemSelectionModel::ClearAndSelect);

  ///FIXME: this won't work remotely
  QObject *obj = ui->sceneComboBox->itemData(index, ObjectModel::ObjectRole).value<QObject*>();
  QGraphicsScene *scene = qobject_cast<QGraphicsScene*>(obj);
  cout << Q_FUNC_INFO << ' ' << scene << ' ' << obj << endl;

  if (scene) {
    ui->graphicsSceneView->setGraphicsScene(scene);
  }
}

void SceneInspectorWidget::sceneItemSelected(const QItemSelection &selection)
{
  QModelIndex index;
  if (!selection.isEmpty())
    index = selection.first().topLeft();

  ///FIXME: this won't work remotely
  if (index.isValid()) {
    QGraphicsItem *item = index.data(SceneModel::SceneItemRole).value<QGraphicsItem*>();
    ui->graphicsSceneView->showGraphicsItem(item);
    ui->sceneTreeView->scrollTo(index); // in case selection does not come from us
  }
}

#include "sceneinspectorwidget.moc"
