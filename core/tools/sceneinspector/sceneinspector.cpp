/*
  sceneinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sceneinspector.h"
#include "ui_sceneinspector.h"

#include "scenemodel.h"

#include <objectmodel.h>
#include <objecttypefilterproxymodel.h>
#include <singlecolumnobjectproxymodel.h>
#include <probeinterface.h>
#include <kde/krecursivefilterproxymodel.h>
#include <metaobjectrepository.h>

#include <QDebug>
#include <QGraphicsItem>
#include <qgraphicsview.h>

using namespace GammaRay;

SceneInspector::SceneInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::SceneInspector)
{
  ui->setupUi(this);

  connect(probe->probe(), SIGNAL(widgetSelected(QWidget*,QPoint)),
          SLOT(widgetSelected(QWidget*,QPoint)));

  ObjectTypeFilterProxyModel<QGraphicsScene> *sceneFilterProxy =
    new ObjectTypeFilterProxyModel<QGraphicsScene>(this);
  sceneFilterProxy->setSourceModel(probe->objectListModel());
  SingleColumnObjectProxyModel *singleColumnProxy = new SingleColumnObjectProxyModel(this);
  singleColumnProxy->setSourceModel(sceneFilterProxy);
  ui->sceneComboBox->setModel(singleColumnProxy);
  connect(ui->sceneComboBox, SIGNAL(activated(int)), SLOT(sceneSelected(int)));
  m_sceneModel = new SceneModel(this);
  QSortFilterProxyModel *sceneFilter = new KRecursiveFilterProxyModel(this);
  sceneFilter->setSourceModel(m_sceneModel);
  ui->sceneTreeView->setModel(sceneFilter);
  ui->screneTreeSearchLine->setProxy(sceneFilter);
  connect(ui->sceneTreeView->selectionModel(),
          SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(sceneItemSelected(QModelIndex)));

  if (ui->sceneComboBox->count()) {
    sceneSelected(0);
  }
}

void SceneInspector::sceneSelected(int index)
{
  QObject *obj = ui->sceneComboBox->itemData(index, ObjectModel::ObjectRole).value<QObject*>();
  QGraphicsScene *scene = qobject_cast<QGraphicsScene*>(obj);
//   qDebug() << Q_FUNC_INFO << scene << obj;

  m_sceneModel->setScene(scene);
  ui->graphicsSceneView->setGraphicsScene(scene);
}

void SceneInspector::sceneItemSelected(const QModelIndex &index)
{
  if (index.isValid()) {
    QGraphicsItem *item = index.data(SceneModel::SceneItemRole).value<QGraphicsItem*>();
    QGraphicsObject *obj = item->toGraphicsObject();
    if (obj) {
      ui->scenePropertyWidget->setObject(obj);
    } else {
      ui->scenePropertyWidget->setObject(item, findBestType(item));
    }
    ui->graphicsSceneView->showGraphicsItem(item);
  } else {
    ui->scenePropertyWidget->setObject(0);
  }
}

void SceneInspector::widgetSelected(QWidget *widget, const QPoint &pos)
{
  QGraphicsView *qgv = Util::findParentOfType<QGraphicsView>(widget);
  if (qgv) {
    // TODO: select qgv->scene() first, right now this only works for a single scene
    QGraphicsItem *item = qgv->itemAt(widget->mapTo(qgv, pos));
    if (item) {
      sceneItemSelected(item);
    }
  }
}

void SceneInspector::sceneItemSelected(QGraphicsItem *item)
{
  QAbstractItemModel *model = ui->sceneTreeView->model();
  const QModelIndexList indexList =
    model->match(model->index(0, 0),
                 SceneModel::SceneItemRole,
                 QVariant::fromValue<QGraphicsItem*>(item), 1,
                 Qt::MatchExactly | Qt::MatchRecursive);
  if (indexList.isEmpty()) {
    return;
  }
  const QModelIndex index = indexList.first();
  ui->sceneTreeView->selectionModel()->select(
    index,
    QItemSelectionModel::Select | QItemSelectionModel::Clear |
    QItemSelectionModel::Rows | QItemSelectionModel::Current);
  ui->sceneTreeView->scrollTo(index);
  sceneItemSelected(index);
}

#define QGV_CHECK_TYPE(Class) \
  if (dynamic_cast<Class*>(item) && MetaObjectRepository::instance()->hasMetaObject(#Class)) \
    return QLatin1String(#Class)

QString SceneInspector::findBestType(QGraphicsItem *item)
{
  // keep this in reverse topological order of the class hierarchy!
  // QObject-based types are covered elsewhere, so we don't need those here
  QGV_CHECK_TYPE(QGraphicsEllipseItem);
  QGV_CHECK_TYPE(QGraphicsPathItem);
  QGV_CHECK_TYPE(QGraphicsPolygonItem);
  QGV_CHECK_TYPE(QGraphicsSimpleTextItem);
  QGV_CHECK_TYPE(QGraphicsRectItem);
  QGV_CHECK_TYPE(QAbstractGraphicsShapeItem);
  QGV_CHECK_TYPE(QGraphicsLineItem);
  QGV_CHECK_TYPE(QGraphicsItemGroup);
  QGV_CHECK_TYPE(QGraphicsPixmapItem);

  return QLatin1String("QGraphicsItem");
}

#include "sceneinspector.moc"
