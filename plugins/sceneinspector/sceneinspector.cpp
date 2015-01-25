/*
  sceneinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/propertycontroller.h>
#include <core/varianthandler.h>

#include <core/objecttypefilterproxymodel.h>
#include <core/probeinterface.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/remote/server.h>

#include <kde/krecursivefilterproxymodel.h>
#include <common/objectbroker.h>
#include <common/endpoint.h>
#include <common/metatypedeclarations.h>
#include <common/objectmodel.h>

#include <QGraphicsEffect>
#include <QGraphicsItem>
#include <QGraphicsLayoutItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsWidget>
#include <QGraphicsView>
#include <QItemSelectionModel>

#include <iostream>

using namespace GammaRay;
using namespace std;

Q_DECLARE_METATYPE(QGraphicsEffect *)
Q_DECLARE_METATYPE(QGraphicsItemGroup *)
Q_DECLARE_METATYPE(QGraphicsObject *)
Q_DECLARE_METATYPE(QGraphicsWidget *)
Q_DECLARE_METATYPE(QGraphicsItem::CacheMode)
Q_DECLARE_METATYPE(QGraphicsItem::GraphicsItemFlags)
Q_DECLARE_METATYPE(QGraphicsItem::PanelModality)
Q_DECLARE_METATYPE(QGraphicsPixmapItem::ShapeMode)

SceneInspector::SceneInspector(ProbeInterface *probe, QObject *parent)
  : SceneInspectorInterface(parent),
    m_propertyController(new PropertyController("com.kdab.GammaRay.SceneInspector", this)),
    m_clientConnected(false)
{
  Server::instance()->registerMonitorNotifier(Endpoint::instance()->objectAddress(objectName()), this, "clientConnectedChanged");

  registerGraphicsViewMetaTypes();
  registerVariantHandlers();

  connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)),
          SLOT(objectSelected(QObject*,QPoint)));

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

  if (m_sceneModel->scene()) {
    disconnect(m_sceneModel->scene(), 0, this, 0);
  }

  m_sceneModel->setScene(scene);
  connectToScene();
  // TODO remote support when a different graphics scene was selected
//  ui->graphicsSceneView->setGraphicsScene(scene);
}

void SceneInspector::connectToScene()
{
  QGraphicsScene *scene = m_sceneModel->scene();
  if (!scene || !m_clientConnected) {
    return;
  }

  connect(scene, SIGNAL(sceneRectChanged(QRectF)),
          this, SIGNAL(sceneRectChanged(QRectF)));
  connect(scene, SIGNAL(changed(QList<QRectF>)),
          this, SIGNAL(sceneChanged()));

  initializeGui();
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

void SceneInspector::clientConnectedChanged(bool clientConnected)
{
  m_clientConnected = clientConnected;
  connectToScene();
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

void SceneInspector::objectSelected(QObject *object, const QPoint &pos)
{
  QWidget *widget = qobject_cast<QWidget*>(object);
  QGraphicsView *qgv = Util::findParentOfType<QGraphicsView>(object);
  if (qgv) {
    // TODO: select qgv->scene() first, right now this only works for a single scene
    QGraphicsItem *item = qgv->itemAt(widget ? widget->mapTo(qgv, pos) : pos);
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

void SceneInspector::registerGraphicsViewMetaTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT0(QGraphicsItem);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptDrops,               setAcceptDrops);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptHoverEvents,         setAcceptHoverEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             acceptTouchEvents,         setAcceptTouchEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, Qt::MouseButtons,                 acceptedMouseButtons,      setAcceptedMouseButtons);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           boundingRect);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            boundingRegionGranularity, setBoundingRegionGranularity);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem::CacheMode,         cacheMode);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           childrenBoundingRect);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPainterPath,                     clipPath);
  MO_ADD_PROPERTY_CR(QGraphicsItem, QCursor,                          cursor,                    setCursor);
  MO_ADD_PROPERTY_RO(QGraphicsItem, qreal,                            effectiveOpacity);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             filtersChildEvents,        setFiltersChildEvents);
  MO_ADD_PROPERTY   (QGraphicsItem, QGraphicsItem::GraphicsItemFlags, flags,                     setFlags);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   focusItem);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   focusProxy);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsEffect*,                 graphicsEffect);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItemGroup*,              group);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             hasCursor);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             hasFocus);
  MO_ADD_PROPERTY   (QGraphicsItem, Qt::InputMethodHints,             inputMethodHints,          setInputMethodHints);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isActive);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isClipped);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             isEnabled,                 setEnabled);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isObscured);
#endif
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isPanel);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             isSelected,                setSelected);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isUnderMouse);
  MO_ADD_PROPERTY   (QGraphicsItem, bool,                             isVisible,                 setVisible);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isWidget);
  MO_ADD_PROPERTY_RO(QGraphicsItem, bool,                             isWindow);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            opacity,                   setOpacity);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPainterPath,                     opaqueArea);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   panel);
  MO_ADD_PROPERTY   (QGraphicsItem, QGraphicsItem::PanelModality,     panelModality,             setPanelModality);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   parentItem);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsObject*,                 parentObject);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsWidget*,                 parentWidget);
  MO_ADD_PROPERTY_CR(QGraphicsItem, QPointF,                          pos,                       setPos);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            rotation,                  setRotation);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            scale,                     setScale);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QRectF,                           sceneBoundingRect);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPointF,                          scenePos);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QTransform,                       sceneTransform);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QPainterPath,                     shape);
  MO_ADD_PROPERTY_CR(QGraphicsItem, QString,                          toolTip,                   setToolTip);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsItem*,                   topLevelItem);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsWidget*,                 topLevelWidget);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QTransform,                       transform/*,                 setTransform*/); // TODO: support setTransform
  MO_ADD_PROPERTY_CR(QGraphicsItem, QPointF,                          transformOriginPoint,      setTransformOriginPoint);
  MO_ADD_PROPERTY_RO(QGraphicsItem, int,                              type);
  MO_ADD_PROPERTY_RO(QGraphicsItem, QGraphicsWidget*,                 window);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            x,                         setX);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            y,                         setY);
  MO_ADD_PROPERTY   (QGraphicsItem, qreal,                            zValue,                    setZValue);

  MO_ADD_METAOBJECT1(QAbstractGraphicsShapeItem, QGraphicsItem);
  MO_ADD_PROPERTY_CR(QAbstractGraphicsShapeItem, QBrush, brush, setBrush);
  MO_ADD_PROPERTY_CR(QAbstractGraphicsShapeItem, QPen,   pen,   setPen);

  MO_ADD_METAOBJECT1(QGraphicsEllipseItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY_CR(QGraphicsEllipseItem, QRectF, rect,    setRect);
  MO_ADD_PROPERTY   (QGraphicsEllipseItem, int, spanAngle,  setSpanAngle);
  MO_ADD_PROPERTY   (QGraphicsEllipseItem, int, startAngle, setStartAngle);

  MO_ADD_METAOBJECT1(QGraphicsPathItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY_CR(QGraphicsPathItem, QPainterPath, path, setPath);

  MO_ADD_METAOBJECT1(QGraphicsPolygonItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY   (QGraphicsPolygonItem, Qt::FillRule, fillRule, setFillRule);
  MO_ADD_PROPERTY_CR(QGraphicsPolygonItem, QPolygonF, polygon, setPolygon);

  MO_ADD_METAOBJECT1(QGraphicsSimpleTextItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY_CR(QGraphicsSimpleTextItem, QFont, font, setFont);
  MO_ADD_PROPERTY_CR(QGraphicsSimpleTextItem, QString, text, setText);

  MO_ADD_METAOBJECT1(QGraphicsRectItem, QAbstractGraphicsShapeItem);
  MO_ADD_PROPERTY_CR(QGraphicsRectItem, QRectF, rect, setRect);

  MO_ADD_METAOBJECT1(QGraphicsLineItem, QGraphicsItem);
  MO_ADD_PROPERTY_CR(QGraphicsLineItem, QLineF, line, setLine);
  MO_ADD_PROPERTY_CR(QGraphicsLineItem, QPen, pen, setPen);

  MO_ADD_METAOBJECT1(QGraphicsPixmapItem, QGraphicsItem);
  MO_ADD_PROPERTY_CR(QGraphicsPixmapItem, QPointF, offset, setOffset);
  MO_ADD_PROPERTY_CR(QGraphicsPixmapItem, QPixmap, pixmap, setPixmap);
  MO_ADD_PROPERTY   (QGraphicsPixmapItem, QGraphicsPixmapItem::ShapeMode, shapeMode, setShapeMode);
  MO_ADD_PROPERTY   (QGraphicsPixmapItem, Qt::TransformationMode, transformationMode, setTransformationMode);

  // no extra properties, but we need the inheritance connection for anything above to work
  MO_ADD_METAOBJECT2(QGraphicsObject, QGraphicsItem, QObject);

  MO_ADD_METAOBJECT0(QGraphicsLayoutItem);
  MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, QRectF, contentsRect);
  MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, bool, isLayout);
  MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, bool, ownedByLayout);

  MO_ADD_METAOBJECT2(QGraphicsWidget, QGraphicsObject, QGraphicsLayoutItem);
  MO_ADD_PROPERTY_RO(QGraphicsWidget, QRectF, windowFrameGeometry);
  MO_ADD_PROPERTY_RO(QGraphicsWidget, QRectF, windowFrameRect);

  MO_ADD_METAOBJECT1(QGraphicsProxyWidget, QGraphicsWidget);
  MO_ADD_PROPERTY_RO(QGraphicsProxyWidget, QWidget*, widget);
}

void SceneInspector::registerVariantHandlers()
{
  VariantHandler::registerStringConverter<QGraphicsItem*>(Util::addressToString);
  VariantHandler::registerStringConverter<QGraphicsItemGroup*>(Util::addressToString);

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  VariantHandler::registerStringConverter<QGraphicsEffect*>(Util::displayString);
  VariantHandler::registerStringConverter<QGraphicsObject*>(Util::displayString);
  VariantHandler::registerStringConverter<QGraphicsWidget*>(Util::displayString);
#endif
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(SceneInspectorFactory)
#endif
