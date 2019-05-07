/*
  sceneinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "sceneinspector.h"

#include "scenemodel.h"
#include "paintanalyzerextension.h"

#include <core/metaenum.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/propertycontroller.h>
#include <core/varianthandler.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/remote/server.h>
#include <core/remote/serverproxymodel.h>
#include <core/propertycontrollerextension.h>

#include <common/objectbroker.h>
#include <common/endpoint.h>
#include <common/metatypedeclarations.h>
#include <common/objectmodel.h>

#include <kde/krecursivefilterproxymodel.h>

#include <QGraphicsEffect>
#include <QGraphicsItem>
#include <QGraphicsLayout>
#include <QGraphicsLayoutItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsWidget>
#include <QGraphicsView>
#include <QItemSelectionModel>
#include <QTextDocument>

#include <iostream>

using namespace GammaRay;
using namespace std;

Q_DECLARE_METATYPE(QGraphicsEffect *)
Q_DECLARE_METATYPE(QGraphicsItemGroup *)
Q_DECLARE_METATYPE(QGraphicsLayoutItem *)
Q_DECLARE_METATYPE(QGraphicsLayout *)
Q_DECLARE_METATYPE(QGraphicsObject *)
Q_DECLARE_METATYPE(QGraphicsWidget *)
Q_DECLARE_METATYPE(QGraphicsItem::CacheMode)
Q_DECLARE_METATYPE(QGraphicsItem::GraphicsItemFlags)
Q_DECLARE_METATYPE(QGraphicsItem::PanelModality)
Q_DECLARE_METATYPE(QGraphicsPixmapItem::ShapeMode)

SceneInspector::SceneInspector(Probe *probe, QObject *parent)
    : SceneInspectorInterface(parent)
    , m_propertyController(new PropertyController(QStringLiteral("com.kdab.GammaRay.SceneInspector"),
                                                  this))
    , m_clientConnected(false)
{
    Server::instance()->registerMonitorNotifier(Endpoint::instance()->objectAddress(
                                                    objectName()), this, "clientConnectedChanged");

    PropertyController::registerExtension<PaintAnalyzerExtension>();

    registerGraphicsViewMetaTypes();
    registerVariantHandlers();

    connect(probe, &Probe::objectSelected,
            this, &SceneInspector::qObjectSelected);
    connect(probe, &Probe::nonQObjectSelected,
            this, &SceneInspector::nonQObjectSelected);

    auto *sceneFilterProxy
        = new ObjectTypeFilterProxyModel<QGraphicsScene>(this);
    sceneFilterProxy->setSourceModel(probe->objectListModel());
    auto *singleColumnProxy = new SingleColumnObjectProxyModel(this);
    singleColumnProxy->setSourceModel(sceneFilterProxy);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.SceneList"), singleColumnProxy);

    QItemSelectionModel *sceneSelection = ObjectBroker::selectionModel(singleColumnProxy);
    connect(sceneSelection, &QItemSelectionModel::selectionChanged,
            this, &SceneInspector::sceneSelected);

    m_sceneModel = new SceneModel(this);
    auto sceneProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    sceneProxy->setSourceModel(m_sceneModel);
    sceneProxy->addRole(ObjectModel::ObjectIdRole);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.SceneGraphModel"), sceneProxy);
    m_itemSelectionModel = ObjectBroker::selectionModel(sceneProxy);
    connect(m_itemSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &SceneInspector::sceneItemSelectionChanged);
}

void SceneInspector::sceneSelected(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    const QModelIndex index = selection.first().topLeft();

    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
    QGraphicsScene *scene = qobject_cast<QGraphicsScene *>(obj);

    if (m_sceneModel->scene())
        disconnect(m_sceneModel->scene(), nullptr, this, nullptr);

    m_sceneModel->setScene(scene);
    connectToScene();
    // TODO remote support when a different graphics scene was selected
// ui->graphicsSceneView->setGraphicsScene(scene);
}

void SceneInspector::connectToScene()
{
    QGraphicsScene *scene = m_sceneModel->scene();
    if (!scene || !m_clientConnected)
        return;

    connect(scene, &QGraphicsScene::sceneRectChanged,
            this, &SceneInspectorInterface::sceneRectChanged);
    connect(scene, &QGraphicsScene::changed,
            this, &SceneInspectorInterface::sceneChanged);

    initializeGui();
}

void SceneInspector::initializeGui()
{
    if (!Endpoint::isConnected()) {
        // only do something if we are connected to a remote client
        return;
    }

    QGraphicsScene *scene = m_sceneModel->scene();
    if (!scene)
        return;

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
    if (!scene)
        return;

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

    QGraphicsItem *currentItem
        = m_itemSelectionModel->currentIndex().data(SceneModel::SceneItemRole).value<QGraphicsItem *>();
    if (currentItem)
        paintItemDecoration(currentItem, transform, &painter);

    emit sceneRendered(view);
}

void SceneInspector::sceneItemSelectionChanged(const QItemSelection &selection)
{
    QModelIndex index;
    if (!selection.isEmpty())
        index = selection.first().topLeft();

    if (index.isValid()) {
        QGraphicsItem *item = index.data(SceneModel::SceneItemRole).value<QGraphicsItem *>();
        QGraphicsObject *obj = item->toGraphicsObject();
        if (obj)
            m_propertyController->setObject(obj);
        else
            m_propertyController->setObject(item, QStringLiteral("QGraphicsItem"));
        emit itemSelected(item->mapRectToScene(item->boundingRect()));
    } else {
        m_propertyController->setObject(nullptr);
        emit sceneChanged();
    }
}

void SceneInspector::qObjectSelected(QObject *object, const QPoint &pos)
{
    QWidget *widget = qobject_cast<QWidget *>(object);
    QGraphicsView *qgv = Util::findParentOfType<QGraphicsView>(object);
    if (qgv) {
        // TODO: select qgv->scene() first, right now this only works for a single scene
        QGraphicsItem *item = qgv->itemAt(widget ? widget->mapTo(qgv, pos) : pos);
        if (item)
            sceneItemSelected(item);
    }

    if (auto item = qobject_cast<QGraphicsObject *>(object))
        sceneItemSelected(item);
}

void SceneInspector::nonQObjectSelected(void *obj, const QString &typeName)
{
    if (typeName == QLatin1String("QGraphicsItem*"))   // TODO: can we get sub-classes here?
        sceneItemSelected(reinterpret_cast<QGraphicsItem *>(obj));
}

void SceneInspector::sceneItemSelected(QGraphicsItem *item)
{
    const auto indexList = m_itemSelectionModel->model()->match(
        m_itemSelectionModel->model()->index(0, 0),
        SceneModel::SceneItemRole,
        QVariant::fromValue<QGraphicsItem *>(item), 1,
        Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;
    const QModelIndex index = indexList.first();
    m_itemSelectionModel->setCurrentIndex(index,
                                          QItemSelectionModel::ClearAndSelect
                                          | QItemSelectionModel::Rows);
}

void SceneInspector::sceneClicked(const QPointF &pos)
{
    QGraphicsItem *item = m_sceneModel->scene()->itemAt(pos, QTransform());
    if (item)
        sceneItemSelected(item);
}

void SceneInspector::registerGraphicsViewMetaTypes()
{
    qRegisterMetaType<QGraphicsEffect *>();
    qRegisterMetaType<QGraphicsLayout *>();

    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT0(QGraphicsItem);
    MO_ADD_PROPERTY(QGraphicsItem, acceptDrops, setAcceptDrops);
    MO_ADD_PROPERTY(QGraphicsItem, acceptHoverEvents, setAcceptHoverEvents);
    MO_ADD_PROPERTY(QGraphicsItem, acceptTouchEvents, setAcceptTouchEvents);
    MO_ADD_PROPERTY(QGraphicsItem, acceptedMouseButtons, setAcceptedMouseButtons);
    MO_ADD_PROPERTY_RO(QGraphicsItem, boundingRect);
    MO_ADD_PROPERTY(QGraphicsItem, boundingRegionGranularity, setBoundingRegionGranularity);
    MO_ADD_PROPERTY_RO(QGraphicsItem, cacheMode);
    MO_ADD_PROPERTY_RO(QGraphicsItem, childrenBoundingRect);
    MO_ADD_PROPERTY_RO(QGraphicsItem, clipPath);
    MO_ADD_PROPERTY(QGraphicsItem, cursor, setCursor);
    MO_ADD_PROPERTY_RO(QGraphicsItem, effectiveOpacity);
    MO_ADD_PROPERTY(QGraphicsItem, filtersChildEvents, setFiltersChildEvents);
    MO_ADD_PROPERTY(QGraphicsItem, flags, setFlags);
    MO_ADD_PROPERTY_RO(QGraphicsItem, focusItem);
    MO_ADD_PROPERTY_RO(QGraphicsItem, focusProxy);
    MO_ADD_PROPERTY_RO(QGraphicsItem, graphicsEffect);
    MO_ADD_PROPERTY_RO(QGraphicsItem, group);
    MO_ADD_PROPERTY_RO(QGraphicsItem, hasCursor);
    MO_ADD_PROPERTY_RO(QGraphicsItem, hasFocus);
    MO_ADD_PROPERTY(QGraphicsItem, inputMethodHints, setInputMethodHints);
    MO_ADD_PROPERTY_RO(QGraphicsItem, isActive);
    MO_ADD_PROPERTY_RO(QGraphicsItem, isClipped);
    MO_ADD_PROPERTY(QGraphicsItem, isEnabled, setEnabled);
    MO_ADD_PROPERTY_RO(QGraphicsItem, isPanel);
    MO_ADD_PROPERTY(QGraphicsItem, isSelected, setSelected);
    MO_ADD_PROPERTY_RO(QGraphicsItem, isUnderMouse);
    MO_ADD_PROPERTY(QGraphicsItem, isVisible, setVisible);
    MO_ADD_PROPERTY_RO(QGraphicsItem, isWidget);
    MO_ADD_PROPERTY_RO(QGraphicsItem, isWindow);
    MO_ADD_PROPERTY(QGraphicsItem, opacity, setOpacity);
    MO_ADD_PROPERTY_RO(QGraphicsItem, opaqueArea);
    MO_ADD_PROPERTY_RO(QGraphicsItem, panel);
    MO_ADD_PROPERTY(QGraphicsItem, panelModality, setPanelModality);
    MO_ADD_PROPERTY_RO(QGraphicsItem, parentItem);
    MO_ADD_PROPERTY_RO(QGraphicsItem, parentObject);
    MO_ADD_PROPERTY_RO(QGraphicsItem, parentWidget);
    MO_ADD_PROPERTY(QGraphicsItem, pos, setPos);
    MO_ADD_PROPERTY(QGraphicsItem, rotation, setRotation);
    MO_ADD_PROPERTY(QGraphicsItem, scale, setScale);
    MO_ADD_PROPERTY_RO(QGraphicsItem, sceneBoundingRect);
    MO_ADD_PROPERTY_RO(QGraphicsItem, scenePos);
    MO_ADD_PROPERTY_RO(QGraphicsItem, sceneTransform);
    MO_ADD_PROPERTY_RO(QGraphicsItem, shape);
    MO_ADD_PROPERTY(QGraphicsItem, toolTip, setToolTip);
    MO_ADD_PROPERTY_RO(QGraphicsItem, topLevelItem);
    MO_ADD_PROPERTY_RO(QGraphicsItem, topLevelWidget);
    MO_ADD_PROPERTY_RO(QGraphicsItem, transform /*,                 setTransform*/);                    // TODO: support setTransform
    MO_ADD_PROPERTY(QGraphicsItem, transformOriginPoint, setTransformOriginPoint);
    MO_ADD_PROPERTY_RO(QGraphicsItem, type);
    MO_ADD_PROPERTY_RO(QGraphicsItem, window);
    MO_ADD_PROPERTY(QGraphicsItem, x, setX);
    MO_ADD_PROPERTY(QGraphicsItem, y, setY);
    MO_ADD_PROPERTY(QGraphicsItem, zValue, setZValue);

    MO_ADD_METAOBJECT1(QAbstractGraphicsShapeItem, QGraphicsItem);
    MO_ADD_PROPERTY(QAbstractGraphicsShapeItem, brush, setBrush);
    MO_ADD_PROPERTY(QAbstractGraphicsShapeItem, pen, setPen);

    MO_ADD_METAOBJECT1(QGraphicsEllipseItem, QAbstractGraphicsShapeItem);
    MO_ADD_PROPERTY(QGraphicsEllipseItem, rect, setRect);
    MO_ADD_PROPERTY(QGraphicsEllipseItem, spanAngle, setSpanAngle);
    MO_ADD_PROPERTY(QGraphicsEllipseItem, startAngle, setStartAngle);

    MO_ADD_METAOBJECT1(QGraphicsPathItem, QAbstractGraphicsShapeItem);
    MO_ADD_PROPERTY(QGraphicsPathItem, path, setPath);

    MO_ADD_METAOBJECT1(QGraphicsPolygonItem, QAbstractGraphicsShapeItem);
    MO_ADD_PROPERTY(QGraphicsPolygonItem, fillRule, setFillRule);
    MO_ADD_PROPERTY(QGraphicsPolygonItem, polygon, setPolygon);

    MO_ADD_METAOBJECT1(QGraphicsSimpleTextItem, QAbstractGraphicsShapeItem);
    MO_ADD_PROPERTY(QGraphicsSimpleTextItem, font, setFont);
    MO_ADD_PROPERTY(QGraphicsSimpleTextItem, text, setText);

    MO_ADD_METAOBJECT1(QGraphicsRectItem, QAbstractGraphicsShapeItem);
    MO_ADD_PROPERTY(QGraphicsRectItem, rect, setRect);

    MO_ADD_METAOBJECT1(QGraphicsLineItem, QGraphicsItem);
    MO_ADD_PROPERTY(QGraphicsLineItem, line, setLine);
    MO_ADD_PROPERTY(QGraphicsLineItem, pen, setPen);

    MO_ADD_METAOBJECT1(QGraphicsPixmapItem, QGraphicsItem);
    MO_ADD_PROPERTY(QGraphicsPixmapItem, offset, setOffset);
    MO_ADD_PROPERTY(QGraphicsPixmapItem, pixmap, setPixmap);
    MO_ADD_PROPERTY(QGraphicsPixmapItem, shapeMode, setShapeMode);
    MO_ADD_PROPERTY(QGraphicsPixmapItem, transformationMode, setTransformationMode);

    // no extra properties, but we need the inheritance connection for anything above to work
    MO_ADD_METAOBJECT2(QGraphicsObject, QObject, QGraphicsItem);

    MO_ADD_METAOBJECT1(QGraphicsTextItem, QGraphicsObject);
    MO_ADD_PROPERTY(QGraphicsTextItem, defaultTextColor, setDefaultTextColor);
    MO_ADD_PROPERTY(QGraphicsTextItem, document, setDocument);
    MO_ADD_PROPERTY(QGraphicsTextItem, font, setFont);
    MO_ADD_PROPERTY(QGraphicsTextItem, tabChangesFocus, setTabChangesFocus);
    MO_ADD_PROPERTY(QGraphicsTextItem, textInteractionFlags, setTextInteractionFlags);
    MO_ADD_PROPERTY(QGraphicsTextItem, textWidth, setTextWidth);

    MO_ADD_METAOBJECT0(QGraphicsLayoutItem);
    MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, contentsRect);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, geometry, setGeometry);
    MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, graphicsItem);
    MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, isLayout);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, maximumHeight, setMaximumHeight);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, maximumSize, setMaximumSize);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, maximumWidth, setMaximumWidth);
    MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, ownedByLayout);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, minimumHeight, setMinimumHeight);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, minimumSize, setMinimumSize);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, minimumWidth, setMinimumWidth);
    MO_ADD_PROPERTY_RO(QGraphicsLayoutItem, parentLayoutItem);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, preferredHeight, setPreferredHeight);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, preferredSize, setPreferredSize);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, preferredWidth, setPreferredWidth);
    MO_ADD_PROPERTY(QGraphicsLayoutItem, sizePolicy, setSizePolicy);

    MO_ADD_METAOBJECT1(QGraphicsLayout, QGraphicsLayoutItem);
    MO_ADD_PROPERTY_RO(QGraphicsLayout, count);
    MO_ADD_PROPERTY_RO(QGraphicsLayout, isActivated);

    MO_ADD_METAOBJECT2(QGraphicsWidget, QGraphicsObject, QGraphicsLayoutItem);
    MO_ADD_PROPERTY_RO(QGraphicsWidget, windowFrameGeometry);
    MO_ADD_PROPERTY_RO(QGraphicsWidget, windowFrameRect);

    MO_ADD_METAOBJECT1(QGraphicsProxyWidget, QGraphicsWidget);
    MO_ADD_PROPERTY_RO(QGraphicsProxyWidget, widget);
}

#define E(x) { QGraphicsItem:: x, #x }
static const MetaEnum::Value<QGraphicsItem::GraphicsItemFlag> graphics_item_flags_table[] = {
    E(ItemIsMovable),
    E(ItemIsSelectable),
    E(ItemIsFocusable),
    E(ItemClipsToShape),
    E(ItemClipsChildrenToShape),
    E(ItemIgnoresTransformations),
    E(ItemIgnoresParentOpacity),
    E(ItemDoesntPropagateOpacityToChildren),
    E(ItemStacksBehindParent),
    E(ItemUsesExtendedStyleOption),
    E(ItemHasNoContents),
    E(ItemSendsGeometryChanges),
    E(ItemAcceptsInputMethod),
    E(ItemNegativeZStacksBehindParent),
    E(ItemIsPanel),
    E(ItemIsFocusScope),
    E(ItemSendsScenePositionChanges),
    E(ItemStopsClickFocusPropagation),
    E(ItemStopsFocusHandling),
    E(ItemContainsChildrenInShape)
};

static QString graphicsItemFlagsToString(QGraphicsItem::GraphicsItemFlags flags)
{
    return MetaEnum::flagsToString(flags, graphics_item_flags_table);
}

static const MetaEnum::Value<QGraphicsItem::CacheMode> graphics_item_cache_mode_table[] = {
    E(NoCache),
    E(ItemCoordinateCache),
    E(DeviceCoordinateCache)
};

static QString graphicsItemCacheModeToString(QGraphicsItem::CacheMode mode)
{
    return MetaEnum::enumToString(mode, graphics_item_cache_mode_table);
}

static const MetaEnum::Value<QGraphicsItem::PanelModality> graphics_item_panel_modality_table[] = {
    E(NonModal),
    E(PanelModal),
    E(SceneModal)
};

static QString graphicsItemPanelModalityToString(QGraphicsItem::PanelModality modality)
{
    return MetaEnum::enumToString(modality, graphics_item_panel_modality_table);
}

#undef E

void SceneInspector::registerVariantHandlers()
{
    VariantHandler::registerStringConverter<QGraphicsItem *>(Util::addressToString);
    VariantHandler::registerStringConverter<QGraphicsItemGroup *>(Util::addressToString);
    VariantHandler::registerStringConverter<QGraphicsLayoutItem *>(Util::addressToString);
    VariantHandler::registerStringConverter<QGraphicsLayout *>(Util::addressToString);

    VariantHandler::registerStringConverter<QGraphicsItem::GraphicsItemFlags>(
        graphicsItemFlagsToString);
    VariantHandler::registerStringConverter<QGraphicsItem::CacheMode>(graphicsItemCacheModeToString);
    VariantHandler::registerStringConverter<QGraphicsItem::PanelModality>(
        graphicsItemPanelModalityToString);
}
