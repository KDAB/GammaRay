/*
  sceneinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "scenemodel.h"

#include <core/metaobjectrepository.h>
#include <core/propertycontroller.h>

#include "include/objectmodel.h"
#include "include/objecttypefilterproxymodel.h"
#include "include/probeinterface.h"
#include "include/singlecolumnobjectproxymodel.h"

#include <kde/krecursivefilterproxymodel.h>
#include <common/network/objectbroker.h>
#include <common/network/endpoint.h>

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QItemSelectionModel>

#include <iostream>

using namespace GammaRay;
using namespace std;

SceneInspector::SceneInspector(ProbeInterface *probe, QObject *parent)
  : SceneInspectorInterface(parent),
    m_propertyController(new PropertyController("com.kdab.GammaRay.SceneInspector", this))
{
  connect(probe->probe(), SIGNAL(widgetSelected(QWidget*,QPoint)),
          SLOT(widgetSelected(QWidget*,QPoint)));

  ObjectTypeFilterProxyModel<QGraphicsScene> *sceneFilterProxy =
    new ObjectTypeFilterProxyModel<QGraphicsScene>(this);
  sceneFilterProxy->setSourceModel(probe->objectListModel());
  SingleColumnObjectProxyModel *singleColumnProxy = new SingleColumnObjectProxyModel(this);
  singleColumnProxy->setSourceModel(sceneFilterProxy);
  probe->registerModel("com.kdab.GammaRay.SceneList", singleColumnProxy);

  QItemSelectionModel* sceneSelection = ObjectBroker::selectionModel(singleColumnProxy);
  connect(sceneSelection, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(sceneSelected(QItemSelection)));

  m_sceneModel = new SceneModel(this);
  probe->registerModel("com.kdab.GammaRay.SceneGraphModel", m_sceneModel);
  m_itemSelectionModel = ObjectBroker::selectionModel(m_sceneModel);
  connect(m_itemSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(sceneItemSelected(QItemSelection)));

  if (singleColumnProxy->rowCount()) {
    sceneSelection->setCurrentIndex(singleColumnProxy->index(0, 0), QItemSelectionModel::ClearAndSelect);
  }
}

void SceneInspector::sceneSelected(const QItemSelection& selection)
{
  if (selection.isEmpty())
    return;
  const QModelIndex index = selection.first().topLeft();

  QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
  QGraphicsScene *scene = qobject_cast<QGraphicsScene*>(obj);
  cout << Q_FUNC_INFO << ' ' << scene << ' ' << obj << endl;

  if (m_sceneModel->scene()) {
    disconnect(m_sceneModel->scene(), 0, this, 0);
  }

  m_sceneModel->setScene(scene);

  connect(scene, SIGNAL(sceneRectChanged(QRectF)),
          this, SIGNAL(sceneRectChanged(QRectF)));
  connect(scene, SIGNAL(changed(QList<QRectF>)),
          this, SIGNAL(sceneChanged()));

  initializeGui();

  // TODO remote support when a different graphics scene was selected
//  ui->graphicsSceneView->setGraphicsScene(scene);
}

void SceneInspector::initializeGui()
{
  if (!Endpoint::isConnected()) {
    // only do something if we are connected to a remote client
    return;
  }

  QGraphicsScene *scene = m_sceneModel->scene();
  if (!scene) {
    return;
  }

  emit sceneRectChanged(scene->sceneRect());
}

void SceneInspector::renderScene(const QTransform &transform, const QSize &size)
{
  if (!Endpoint::isConnected()) {
    // only do something if we are connected to a remote client
    return;
  }

  QGraphicsScene *scene = m_sceneModel->scene();
  if (!scene) {
    return;
  }

  // initialize transparent pixmap
  QPixmap view(size);
  view.fill(Qt::transparent);

  // setup painter and apply transformation of client view
  QPainter painter(&view);
  painter.setWorldTransform(transform);

  // the area we want to paint has the size of the client's viewport _after_ applying
  // the transformation. Thus first apply the inverse to yield the desired area afterwards
  QRectF area(QPointF(0, 0), size);
  area = transform.inverted().mapRect(area);

  scene->render(&painter, area, area, Qt::IgnoreAspectRatio);

  QGraphicsItem *currentItem = m_itemSelectionModel->currentIndex().data(SceneModel::SceneItemRole).value<QGraphicsItem*>();
  if (currentItem) {
    paintItemDecoration(currentItem, transform, &painter);
  }

  emit sceneRendered(view);
}

void SceneInspector::sceneItemSelected(const QItemSelection& selection)
{
  QModelIndex index;
  if (!selection.isEmpty())
    index = selection.first().topLeft();

  if (index.isValid()) {
    QGraphicsItem *item = index.data(SceneModel::SceneItemRole).value<QGraphicsItem*>();
    QGraphicsObject *obj = item->toGraphicsObject();
    if (obj) {
      m_propertyController->setObject(obj);
    } else {
      m_propertyController->setObject(item, findBestType(item));
    }
    emit itemSelected(item->mapRectToScene(item->boundingRect()));
  } else {
    m_propertyController->setObject(0);
    emit sceneChanged();
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
  const QModelIndexList indexList =
    m_sceneModel->match(m_sceneModel->index(0, 0),
                 SceneModel::SceneItemRole,
                 QVariant::fromValue<QGraphicsItem*>(item), 1,
                 Qt::MatchExactly | Qt::MatchRecursive);
  if (indexList.isEmpty()) {
    return;
  }
  const QModelIndex index = indexList.first();
  m_itemSelectionModel->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

void SceneInspector::sceneClicked(const QPointF &pos)
{
  QGraphicsItem *item = m_sceneModel->scene()->itemAt(pos);
  if (item) {
    sceneItemSelected(item);
  }
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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(SceneInspectorFactory)
#endif

#include "sceneinspector.moc"
