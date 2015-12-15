/*
  qmlsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickinspector.h"
#include "quickitemmodel.h"
#include "quickscenegraphmodel.h"
#include "transferimage.h"
#include "geometryextension/sggeometryextension.h"
#include "materialextension/materialextension.h"

#include <common/modelevent.h>
#include <common/objectbroker.h>

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/probeinterface.h>
#include <core/propertycontroller.h>
#include <core/remote/server.h>
#include <core/remote/serverproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/varianthandler.h>

#include <3rdparty/kde/krecursivefilterproxymodel.h>

#include <QQuickItem>
#include <QQuickWindow>
#include <QQuickView>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlError>

#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QPainter>
#include <QSGNode>
#include <QSGGeometry>
#include <QSGMaterial>
#include <QSGFlatColorMaterial>
#include <QSGTextureMaterial>
#include <QSGVertexColorMaterial>
#include <private/qquickshadereffectsource_p.h>
#include <QMatrix4x4>
#include <QCoreApplication>

#include <private/qquickanchors_p.h>
#include <private/qquickitem_p.h>
#include <private/qsgbatchrenderer_p.h>

Q_DECLARE_METATYPE(QQmlError)

Q_DECLARE_METATYPE(QQuickItem::Flags)
Q_DECLARE_METATYPE(QQuickPaintedItem::PerformanceHints)
Q_DECLARE_METATYPE(QSGNode *)
Q_DECLARE_METATYPE(QSGBasicGeometryNode *)
Q_DECLARE_METATYPE(QSGGeometryNode *)
Q_DECLARE_METATYPE(QSGClipNode *)
Q_DECLARE_METATYPE(QSGTransformNode *)
Q_DECLARE_METATYPE(QSGRootNode *)
Q_DECLARE_METATYPE(QSGOpacityNode *)
Q_DECLARE_METATYPE(QSGNode::Flags)
Q_DECLARE_METATYPE(QSGNode::DirtyState)
Q_DECLARE_METATYPE(QSGGeometry *)
Q_DECLARE_METATYPE(QMatrix4x4 *)
Q_DECLARE_METATYPE(const QMatrix4x4 *)
Q_DECLARE_METATYPE(const QSGClipNode *)
Q_DECLARE_METATYPE(const QSGGeometry *)
Q_DECLARE_METATYPE(QSGMaterial *)
Q_DECLARE_METATYPE(QSGMaterial::Flags)
Q_DECLARE_METATYPE(QSGTexture::WrapMode)
Q_DECLARE_METATYPE(QSGTexture::Filtering)
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(Qt::MouseButtons)
#endif
using namespace GammaRay;

static QString qQuickItemFlagsToString(QQuickItem::Flags flags)
{
  QStringList list;
  if (flags & QQuickItem::ItemClipsChildrenToShape)
    list << QStringLiteral("ItemClipsChildrenToShape");
  if (flags & QQuickItem::ItemAcceptsInputMethod)
    list << QStringLiteral("ItemAcceptsInputMethod");
  if (flags & QQuickItem::ItemIsFocusScope)
    list << QStringLiteral("ItemIsFocusScope");
  if (flags & QQuickItem::ItemHasContents)
    list << QStringLiteral("ItemHasContents");
  if (flags & QQuickItem::ItemAcceptsDrops)
    list << QStringLiteral("ItemAcceptsDrops");
  if (list.isEmpty())
    return QStringLiteral("<none>");
  return list.join(QStringLiteral(" | "));
}

static QString qQuickPaintedItemPerformanceHintsToString(QQuickPaintedItem::PerformanceHints hints)
{
  QStringList list;
  if (hints & QQuickPaintedItem::FastFBOResizing)
    list << QStringLiteral("FastFBOResizing");
  if (list.isEmpty())
    return QStringLiteral("<none>");
  return list.join(QStringLiteral(" | "));
}

static QString qSGNodeFlagsToString(QSGNode::Flags flags)
{
  QStringList list;
  if (flags & QSGNode::OwnedByParent) {
    list << QStringLiteral("OwnedByParent");
  }
  if (flags & QSGNode::UsePreprocess) {
    list << QStringLiteral("UsePreprocess");
  }
  if (flags & QSGNode::OwnsGeometry) {
    list << QStringLiteral("OwnsGeometry");
  }
  if (flags & QSGNode::OwnsMaterial) {
    list << QStringLiteral("OwnsMaterial");
  }
  if (flags & QSGNode::OwnsOpaqueMaterial) {
    list << QStringLiteral("OwnsOpaqueMaterial");
  }
  if (list.isEmpty())
    return QStringLiteral("<none>");
  return list.join(QStringLiteral(" | "));
}
static QString qSGNodeDirtyStateToString(QSGNode::DirtyState flags)
{
  QStringList list;
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
  if (flags & QSGNode::DirtySubtreeBlocked) {
    list << QStringLiteral("DirtySubtreeBlocked");
  }
#endif
  if (flags & QSGNode::DirtyMatrix) {
    list << QStringLiteral("DirtyMatrix");
  }
  if (flags & QSGNode::DirtyNodeAdded) {
    list << QStringLiteral("DirtyNodeAdded");
  }
  if (flags & QSGNode::DirtyNodeRemoved) {
    list << QStringLiteral("DirtyNodeRemoved");
  }
  if (flags & QSGNode::DirtyGeometry) {
    list << QStringLiteral("DirtyGeometry");
  }
  if (flags & QSGNode::DirtyMaterial) {
    list << QStringLiteral("DirtyMaterial");
  }
  if (flags & QSGNode::DirtyOpacity) {
    list << QStringLiteral("DirtyOpacity");
  }
  if (flags & QSGNode::DirtyForceUpdate) {
    list << QStringLiteral("DirtyForceUpdate");
  }
  if (flags & QSGNode::DirtyUsePreprocess) {
    list << QStringLiteral("DirtyUsePreprocess");
  }
  if (flags & QSGNode::DirtyPropagationMask) {
    list << QStringLiteral("DirtyPropagationMask");
  }
  if (list.isEmpty())
    return QStringLiteral("Clean");
  return list.join(QStringLiteral(" | "));
}

static QString qsgMaterialFlagsToString(QSGMaterial::Flags flags)
{
  QStringList list;
#define F(f) if (flags & QSGMaterial::f) list.push_back(QStringLiteral(#f));
  F(Blending)
  F(RequiresDeterminant)
  F(RequiresFullMatrixExceptTranslate)
  F(RequiresFullMatrix)
  F(CustomCompileStep)
#undef F

  if (list.isEmpty())
    return QStringLiteral("<none>");
  return list.join(QStringLiteral(" | "));
}

static QString qsgTextureFilteringToString(QSGTexture::Filtering filtering)
{
  switch (filtering) {
    case QSGTexture::None: return QStringLiteral("None");
    case QSGTexture::Nearest: return QStringLiteral("Nearest");
    case QSGTexture::Linear: return QStringLiteral("Linear");
  }
  return QStringLiteral("Unknown: %1").arg(filtering);
}

static QString qsgTextureWrapModeToString(QSGTexture::WrapMode wrapMode)
{
  switch (wrapMode) {
    case QSGTexture::Repeat: return QStringLiteral("Repeat");
    case QSGTexture::ClampToEdge: return QStringLiteral("ClampToEdge");
  }
  return QStringLiteral("Unknown: %1").arg(wrapMode);
}

QuickInspector::QuickInspector(ProbeInterface *probe, QObject *parent)
  : QuickInspectorInterface(parent),
    m_probe(probe),
    m_currentSgNode(0),
    m_itemModel(new QuickItemModel(this)),
    m_sgModel(new QuickSceneGraphModel(this)),
    m_itemPropertyController(new PropertyController(QStringLiteral("com.kdab.GammaRay.QuickItem"), this)),
    m_sgPropertyController(new PropertyController(QStringLiteral("com.kdab.GammaRay.QuickSceneGraph"), this)),
    m_clientViewActive(false),
    m_needsNewFrame(false),
    m_isGrabbingWindow(false)
{
  registerPCExtensions();
  Server::instance()->registerMonitorNotifier(
    Endpoint::instance()->objectAddress(objectName()), this, "clientConnectedChanged");

  registerMetaTypes();
  registerVariantHandlers();
  probe->installGlobalEventFilter(this);

  QAbstractProxyModel *windowModel = new ObjectTypeFilterProxyModel<QQuickWindow>(this);
  windowModel->setSourceModel(probe->objectListModel());
  QAbstractProxyModel * proxy = new SingleColumnObjectProxyModel(this);
  proxy->setSourceModel(windowModel);
  m_windowModel = proxy;
  probe->registerModel(QStringLiteral("com.kdab.GammaRay.QuickWindowModel"), m_windowModel);

  auto filterProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
  filterProxy->setSourceModel(m_itemModel);
  probe->registerModel(QStringLiteral("com.kdab.GammaRay.QuickItemModel"), filterProxy);

  connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
          m_itemModel, SLOT(objectAdded(QObject*)));
  connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)),
          m_itemModel, SLOT(objectRemoved(QObject*)));
  connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)),
          SLOT(objectSelected(QObject*)));
  connect(probe->probe(), SIGNAL(nonQObjectSelected(void*,QString)),
          SLOT(objectSelected(void*,QString)));

  m_itemSelectionModel = ObjectBroker::selectionModel(filterProxy);
  connect(m_itemSelectionModel, &QItemSelectionModel::selectionChanged,
          this, &QuickInspector::itemSelectionChanged);

  filterProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
  filterProxy->setSourceModel(m_sgModel);
  probe->registerModel(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"), filterProxy);

  m_sgSelectionModel = ObjectBroker::selectionModel(filterProxy);
  connect(m_sgSelectionModel, &QItemSelectionModel::selectionChanged,
          this, &QuickInspector::sgSelectionChanged);
  connect(m_sgModel, &QuickSceneGraphModel::nodeDeleted, this, &QuickInspector::sgNodeDeleted);
}

QuickInspector::~QuickInspector()
{
}

void QuickInspector::selectWindow(int index)
{
  const QModelIndex mi = m_windowModel->index(index, 0);
  QQuickWindow *window = mi.data(ObjectModel::ObjectRole).value<QQuickWindow*>();
  selectWindow(window);
}

void QuickInspector::selectWindow(QQuickWindow *window)
{
  if (m_window) {
    disconnect(m_window, 0, this, 0);
  }

  m_window = window;
  m_itemModel->setWindow(window);
  m_sgModel->setWindow(window);

  if (m_window) {
    // make sure we have selected something for the property editor to not be entirely empty
    selectItem(m_window->contentItem());

    connect(window, &QQuickWindow::frameSwapped, this, &QuickInspector::slotSceneChanged);

    m_window->update();
  }
}

void QuickInspector::selectItem(QQuickItem *item)
{
  const QAbstractItemModel *model = m_itemSelectionModel->model();
  Model::used(model);

  const QModelIndexList indexList =
    model->match(model->index(0, 0),
                 ObjectModel::ObjectRole,
                 QVariant::fromValue<QQuickItem*>(item), 1,
                 Qt::MatchExactly | Qt::MatchRecursive);
  if (indexList.isEmpty()) {
    return;
  }

  const QModelIndex index = indexList.first();
  m_itemSelectionModel->select(index,
                               QItemSelectionModel::Select |
                               QItemSelectionModel::Clear |
                               QItemSelectionModel::Rows |
                               QItemSelectionModel::Current);
}

void QuickInspector::selectSGNode(QSGNode *node)
{
  const QAbstractItemModel *model = m_sgSelectionModel->model();
  Model::used(model);

  const QModelIndexList indexList = model->match(model->index(0, 0), ObjectModel::ObjectRole,
    QVariant::fromValue(node), 1, Qt::MatchExactly | Qt::MatchRecursive);
  if (indexList.isEmpty()) {
    return;
  }

  const QModelIndex index = indexList.first();
  m_sgSelectionModel->select(index,
                             QItemSelectionModel::Select |
                             QItemSelectionModel::Clear |
                             QItemSelectionModel::Rows |
                             QItemSelectionModel::Current);
}

void QuickInspector::objectSelected(QObject *object)
{
  QQuickItem *item = qobject_cast<QQuickItem*>(object);
  if (item) {
    selectItem(item);
  }
}

void QuickInspector::objectSelected(void *object, const QString &typeName)
{
  if (MetaObjectRepository::instance()->metaObject(typeName)->inherits(QStringLiteral("QSGNode"))) {
    selectSGNode(reinterpret_cast<QSGNode*>(object));
  }
}

void QuickInspector::renderScene()
{
  if (!m_clientViewActive || !m_window) {
    return;
  }

  m_needsNewFrame = true;
  m_window->update();
}

void QuickInspector::sendRenderedScene()
{
  QuickItemGeometry itemGeometry;
  TransferImage rawImage = TransferImage(m_currentFrame); // wrap to allow bypassing expensive PNG compression
  if (m_currentItem) {
    QQuickItem *parent = m_currentItem->parentItem();

    if (parent) {
      itemGeometry.itemRect =  m_currentItem->parentItem()->mapRectToScene(
                           QRectF(m_currentItem->x(), m_currentItem->y(),
                                  m_currentItem->width(), m_currentItem->height()));
    } else {
      itemGeometry.itemRect =  QRectF(0, 0, m_currentItem->width(), m_currentItem->height());
    }

    itemGeometry.boundingRect =
                       m_currentItem->mapRectToScene(m_currentItem->boundingRect());
    itemGeometry.childrenRect =
                       m_currentItem->mapRectToScene(m_currentItem->childrenRect());
    itemGeometry.transformOriginPoint =
                       m_currentItem->mapToScene(m_currentItem->transformOriginPoint());

    QQuickAnchors *anchors = m_currentItem->property("anchors").value<QQuickAnchors*>();

    if (anchors) {
      QQuickAnchors::Anchors usedAnchors = anchors->usedAnchors();
      itemGeometry.left = (bool)(usedAnchors & QQuickAnchors::LeftAnchor) || anchors->fill();
      itemGeometry.right = (bool)(usedAnchors & QQuickAnchors::RightAnchor) || anchors->fill();
      itemGeometry.top = (bool)(usedAnchors & QQuickAnchors::TopAnchor) || anchors->fill();
      itemGeometry.bottom = (bool)(usedAnchors & QQuickAnchors::BottomAnchor) || anchors->fill();
      itemGeometry.baseline = (bool)(usedAnchors & QQuickAnchors::BaselineAnchor);
      itemGeometry.horizontalCenter = (bool)(usedAnchors & QQuickAnchors::HCenterAnchor) || anchors->centerIn();
      itemGeometry.verticalCenter = (bool)(usedAnchors & QQuickAnchors::VCenterAnchor) || anchors->centerIn();
      itemGeometry.leftMargin =  anchors->leftMargin();
      itemGeometry.rightMargin =  anchors->rightMargin();
      itemGeometry.topMargin =  anchors->topMargin();
      itemGeometry.bottomMargin =  anchors->bottomMargin();
      itemGeometry.horizontalCenterOffset =  anchors->horizontalCenterOffset();
      itemGeometry.verticalCenterOffset =  anchors->verticalCenterOffset();
      itemGeometry.baselineOffset =  anchors->baselineOffset();
      itemGeometry.margins = anchors->margins();
    }
    itemGeometry.x =  m_currentItem->x();
    itemGeometry.y =  m_currentItem->y();
    QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(m_currentItem);
    itemGeometry.transform =  itemPriv->itemToWindowTransform();
    if (parent) {
      QQuickItemPrivate *parentPriv = QQuickItemPrivate::get(parent);
      itemGeometry.parentTransform =  parentPriv->itemToWindowTransform();
    }
  }
  emit sceneRendered(rawImage, itemGeometry);
}

void QuickInspector::slotSceneChanged()
{
  if (!m_clientViewActive || !m_window || m_isGrabbingWindow) {
    return;
  }

  if (!m_needsNewFrame) {
    emit sceneChanged();
    return;
  }

  Q_ASSERT(QThread::currentThread() == QCoreApplication::instance()->thread());
  m_isGrabbingWindow = true;
  m_currentFrame = m_window->grabWindow();
  m_isGrabbingWindow = false;

  m_needsNewFrame = false;
  QMetaObject::invokeMethod(this, "sendRenderedScene", Qt::AutoConnection); // we are in the render thread here
}

void QuickInspector::sendKeyEvent(int type, int key, int modifiers, const QString &text,
                                  bool autorep, ushort count)
{
  if (!m_window) {
    return;
  }

  QCoreApplication::sendEvent(m_window,
                              new QKeyEvent((QEvent::Type)type,
                                            key,
                                            (Qt::KeyboardModifiers)modifiers,
                                            text,
                                            autorep,
                                            count));
}

void QuickInspector::sendMouseEvent(int type, const QPointF &localPos, int button,
                                    int buttons, int modifiers)
{
  if (!m_window) {
    return;
  }

  QCoreApplication::sendEvent(m_window,
                              new QMouseEvent((QEvent::Type)type,
                                              localPos,
                                              (Qt::MouseButton)button,
                                              (Qt::MouseButtons)buttons,
                                              (Qt::KeyboardModifiers)modifiers));
}

void QuickInspector::sendWheelEvent(const QPointF &localPos, QPoint pixelDelta, QPoint angleDelta,
                                    int buttons, int modifiers)
{
  if (!m_window) {
    return;
  }

  QCoreApplication::sendEvent(m_window,
                              new QWheelEvent(localPos,
                                              m_window->mapToGlobal(localPos.toPoint()),
                                              pixelDelta,
                                              angleDelta,
                                              0, /*not used*/
                                              Qt::Vertical, /*not used*/
                                              (Qt::MouseButtons)buttons,
                                              (Qt::KeyboardModifiers)modifiers));
}

void QuickInspector::setCustomRenderMode(
  GammaRay::QuickInspectorInterface::RenderMode customRenderMode)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
  QQuickWindowPrivate *winPriv = QQuickWindowPrivate::get(m_window);
  winPriv->customRenderMode = customRenderMode == VisualizeClipping ? "clip"     :
                              customRenderMode == VisualizeOverdraw ? "overdraw" :
                              customRenderMode == VisualizeBatches  ? "batches"  :
                              customRenderMode == VisualizeChanges  ? "changes"  :
                              "";
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0) && 0 // FIXME this crashes on scene updates with Qt >= 5.5.1 and the multithreaded renderer
  // Qt does some performance optimizations that break custom render modes.
  // Thus the optimizations are only applied if there is no custom render mode set.
  // So we need to make the scenegraph recheck whether a custom render mode is set.
  // We do this by simply recreating the renderer.

  QQuickItemPrivate *contentPriv = QQuickItemPrivate::get(m_window->contentItem());
  QSGNode *rootNode = contentPriv->itemNode();
  while(rootNode->parent()) {
      rootNode = rootNode->parent();
  }

  delete winPriv->renderer;
  winPriv->renderer = winPriv->context->createRenderer();
  winPriv->renderer->setRootNode(static_cast<QSGRootNode*>(rootNode));
#endif
  m_window->update();

#else
  Q_UNUSED(customRenderMode);
#endif
}

void QuickInspector::checkFeatures()
{
  emit features(
    Features(
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0) // FIXME any render mode crashes on first scene change
      NoFeatures
#elif QT_VERSION >= QT_VERSION_CHECK(5, 5, 0) // batching crashes when enabled at runtime >= 5.5.0
      CustomRenderModeChanges | CustomRenderModeClipping | CustomRenderModeOverdraw
#elif QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
      AllCustomRenderModes
#endif
    )
  );
}

void QuickInspector::itemSelectionChanged(const QItemSelection &selection)
{
  if (selection.isEmpty()) {
    return;
  }

  const QModelIndex index = selection.first().topLeft();
  m_currentItem = index.data(ObjectModel::ObjectRole).value<QQuickItem*>();
  m_itemPropertyController->setObject(m_currentItem);

  // It might be that a sg-node is already selected that belongs to this item, but isn't the root
  // node of the Item. In this case we don't want to overwrite that selection.
  if (m_sgModel->itemForSgNode(m_currentSgNode) != m_currentItem) {
    m_currentSgNode = m_sgModel->sgNodeForItem(m_currentItem);
    const auto sourceIdx = m_sgModel->indexForNode(m_currentSgNode);
    auto proxy = qobject_cast<const QAbstractProxyModel*>(m_sgSelectionModel->model());
    m_sgSelectionModel->select(proxy->mapFromSource(sourceIdx),
                               QItemSelectionModel::Select |
                               QItemSelectionModel::Clear |
                               QItemSelectionModel::Rows |
                               QItemSelectionModel::Current);
  }

  if (m_window) {
    m_window->update();
  }
}

void QuickInspector::sgSelectionChanged(const QItemSelection &selection)
{
  if (selection.isEmpty()) {
    return;
  }

  const QModelIndex index = selection.first().topLeft();
  m_currentSgNode = index.data(ObjectModel::ObjectRole).value<QSGNode*>();
  if (!m_sgModel->verifyNodeValidity(m_currentSgNode)) {
    return; // Apparently the node has been deleted meanwhile, so don't access it.
  }

  m_sgPropertyController->setObject(m_currentSgNode, findSGNodeType(m_currentSgNode));

  m_currentItem = m_sgModel->itemForSgNode(m_currentSgNode);
  selectItem(m_currentItem);
}

void QuickInspector::sgNodeDeleted(QSGNode *node)
{
  if (m_currentSgNode == node) {
    m_sgPropertyController->setObject(0);
  }
}

void QuickInspector::clientConnectedChanged(bool connected)
{
  if (!connected)
    setSceneViewActive(false);
}

void QuickInspector::setSceneViewActive(bool active)
{
  m_clientViewActive = active;

  if (active && m_window) {
    m_window->update();
  }
}

QQuickItem *QuickInspector::recursiveChiltAt(QQuickItem *parent, const QPointF &pos) const
{
  Q_ASSERT(parent);

  QQuickItem *child = parent->childAt(pos.x(), pos.y());

  if (child) {
    return recursiveChiltAt(child, parent->mapToItem(child, pos));
  }
  return parent;
}

bool QuickInspector::eventFilter(QObject *receiver, QEvent *event)
{
  if (event->type() == QEvent::MouseButtonRelease) {
    QMouseEvent *mouseEv = static_cast<QMouseEvent*>(event);
    if (mouseEv->button() == Qt::LeftButton &&
        mouseEv->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
      QQuickWindow *window = qobject_cast<QQuickWindow*>(receiver);
      if (window && window->contentItem()) {
        QQuickItem *item = recursiveChiltAt(window->contentItem(), mouseEv->pos());
        m_probe->selectObject(item);
      }
    }
  }

  return QObject::eventFilter(receiver, event);
}

void QuickInspector::registerMetaTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT1(QQuickWindow, QWindow);
  MO_ADD_PROPERTY   (QQuickWindow, bool, clearBeforeRendering, setClearBeforeRendering);
  MO_ADD_PROPERTY   (QQuickWindow, bool, isPersistentOpenGLContext, setPersistentOpenGLContext);
  MO_ADD_PROPERTY   (QQuickWindow, bool, isPersistentSceneGraph, setPersistentSceneGraph);
  MO_ADD_PROPERTY_RO(QQuickWindow, QQuickItem *, mouseGrabberItem);
  MO_ADD_PROPERTY_RO(QQuickWindow, QOpenGLContext *, openglContext);
  MO_ADD_PROPERTY_RO(QQuickWindow, uint, renderTargetId);

  MO_ADD_METAOBJECT1(QQuickView, QQuickWindow);
  MO_ADD_PROPERTY_RO(QQuickView, QQmlEngine *, engine);
  MO_ADD_PROPERTY_RO(QQuickView, QList<QQmlError>, errors);
  MO_ADD_PROPERTY_RO(QQuickView, QSize, initialSize);
  MO_ADD_PROPERTY_RO(QQuickView, QQmlContext *, rootContext);
  MO_ADD_PROPERTY_RO(QQuickView, QQuickItem *, rootObject);

  MO_ADD_METAOBJECT1(QQuickItem, QObject);
  MO_ADD_PROPERTY   (QQuickItem, bool, acceptHoverEvents, setAcceptHoverEvents);
  MO_ADD_PROPERTY   (QQuickItem, Qt::MouseButtons, acceptedMouseButtons, setAcceptedMouseButtons);
  MO_ADD_PROPERTY_CR(QQuickItem, QCursor, cursor, setCursor);
  MO_ADD_PROPERTY   (QQuickItem, bool, filtersChildMouseEvents, setFiltersChildMouseEvents);
  MO_ADD_PROPERTY   (QQuickItem, QQuickItem::Flags, flags, setFlags);
  MO_ADD_PROPERTY_RO(QQuickItem, bool, isFocusScope);
  MO_ADD_PROPERTY_RO(QQuickItem, bool, isTextureProvider);
  MO_ADD_PROPERTY   (QQuickItem, bool, keepMouseGrab, setKeepMouseGrab);
  MO_ADD_PROPERTY   (QQuickItem, bool, keepTouchGrab, setKeepTouchGrab);
  //MO_ADD_PROPERTY_RO(QQuickItem, QQuickItem*, nextItemInFocusChain); // FIXME fails on the default argument
  MO_ADD_PROPERTY_RO(QQuickItem, QQuickItem*, scopedFocusItem);
  MO_ADD_PROPERTY_RO(QQuickItem, QQuickWindow*, window);

  MO_ADD_METAOBJECT1(QQuickPaintedItem, QQuickItem);
  MO_ADD_PROPERTY_RO(QQuickPaintedItem, QRectF, contentsBoundingRect);
  MO_ADD_PROPERTY   (QQuickPaintedItem, bool, mipmap, setMipmap);
  MO_ADD_PROPERTY   (QQuickPaintedItem, bool, opaquePainting, setOpaquePainting);
  MO_ADD_PROPERTY   (QQuickPaintedItem, QQuickPaintedItem::PerformanceHints, performanceHints, setPerformanceHints);

  MO_ADD_METAOBJECT1(QSGTexture, QObject);
  MO_ADD_PROPERTY   (QSGTexture, QSGTexture::Filtering, filtering, setFiltering);
  MO_ADD_PROPERTY_RO(QSGTexture, bool, hasAlphaChannel);
  MO_ADD_PROPERTY_RO(QSGTexture, bool, hasMipmaps);
  MO_ADD_PROPERTY   (QSGTexture, QSGTexture::WrapMode, horizontalWrapMode, setHorizontalWrapMode);
  MO_ADD_PROPERTY_RO(QSGTexture, bool, isAtlasTexture);
  MO_ADD_PROPERTY   (QSGTexture, QSGTexture::Filtering, mipmapFiltering, setMipmapFiltering);
  MO_ADD_PROPERTY_RO(QSGTexture, QRectF, normalizedTextureSubRect);
  MO_ADD_PROPERTY_RO(QSGTexture, int, textureId);
  MO_ADD_PROPERTY_RO(QSGTexture, QSize, textureSize);
  MO_ADD_PROPERTY   (QSGTexture, QSGTexture::WrapMode, verticalWrapMode, setVerticalWrapMode);

  MO_ADD_METAOBJECT0(QSGNode);
  MO_ADD_PROPERTY_RO(QSGNode, QSGNode *, parent);
  MO_ADD_PROPERTY_RO(QSGNode, int, childCount);
  MO_ADD_PROPERTY_RO(QSGNode, QSGNode::Flags, flags);
  MO_ADD_PROPERTY_RO(QSGNode, bool, isSubtreeBlocked);
  MO_ADD_PROPERTY   (QSGNode, QSGNode::DirtyState, dirtyState, markDirty);

  MO_ADD_METAOBJECT1(QSGBasicGeometryNode, QSGNode);
  MO_ADD_PROPERTY_RO(QSGBasicGeometryNode, const QSGGeometry *, geometry);
  MO_ADD_PROPERTY_RO(QSGBasicGeometryNode, const QMatrix4x4 *, matrix);
  MO_ADD_PROPERTY_RO(QSGBasicGeometryNode, const QSGClipNode *, clipList);

  MO_ADD_METAOBJECT1(QSGGeometryNode, QSGBasicGeometryNode);
  MO_ADD_PROPERTY   (QSGGeometryNode, QSGMaterial *, material, setMaterial);
  MO_ADD_PROPERTY   (QSGGeometryNode, QSGMaterial *, opaqueMaterial, setOpaqueMaterial);
  MO_ADD_PROPERTY_RO(QSGGeometryNode, QSGMaterial *, activeMaterial);
  MO_ADD_PROPERTY   (QSGGeometryNode, int, renderOrder, setRenderOrder);
  MO_ADD_PROPERTY   (QSGGeometryNode, qreal, inheritedOpacity, setInheritedOpacity);

  MO_ADD_METAOBJECT1(QSGClipNode, QSGBasicGeometryNode);
  MO_ADD_PROPERTY   (QSGClipNode, bool, isRectangular, setIsRectangular);
  MO_ADD_PROPERTY_CR(QSGClipNode, QRectF, clipRect, setClipRect);
  MO_ADD_PROPERTY_RO(QSGClipNode, const QMatrix4x4 *, matrix);
  MO_ADD_PROPERTY_RO(QSGClipNode, const QSGClipNode *, clipList);

  MO_ADD_METAOBJECT1(QSGTransformNode, QSGNode);
  MO_ADD_PROPERTY   (QSGTransformNode, const QMatrix4x4&, matrix, setMatrix);
  MO_ADD_PROPERTY   (QSGTransformNode, const QMatrix4x4&, combinedMatrix, setCombinedMatrix);

  MO_ADD_METAOBJECT1(QSGRootNode, QSGNode);

  MO_ADD_METAOBJECT1(QSGOpacityNode, QSGNode);
  MO_ADD_PROPERTY   (QSGOpacityNode, qreal, opacity, setOpacity);
  MO_ADD_PROPERTY   (QSGOpacityNode, qreal, combinedOpacity, setCombinedOpacity);

  MO_ADD_METAOBJECT0(QSGMaterial);
  MO_ADD_PROPERTY_RO(QSGMaterial, QSGMaterial::Flags, flags);

  MO_ADD_METAOBJECT1(QSGFlatColorMaterial, QSGMaterial);
  MO_ADD_PROPERTY   (QSGFlatColorMaterial, const QColor&, color, setColor);

  MO_ADD_METAOBJECT1(QSGOpaqueTextureMaterial, QSGMaterial);
  MO_ADD_PROPERTY   (QSGOpaqueTextureMaterial, QSGTexture::Filtering, filtering, setFiltering);
  MO_ADD_PROPERTY   (QSGOpaqueTextureMaterial, QSGTexture::WrapMode, horizontalWrapMode, setHorizontalWrapMode);
  MO_ADD_PROPERTY   (QSGOpaqueTextureMaterial, QSGTexture::Filtering, mipmapFiltering, setMipmapFiltering);
  MO_ADD_PROPERTY   (QSGOpaqueTextureMaterial, QSGTexture*, texture, setTexture);
  MO_ADD_PROPERTY   (QSGOpaqueTextureMaterial, QSGTexture::WrapMode, verticalWrapMode, setVerticalWrapMode);
  MO_ADD_METAOBJECT1(QSGTextureMaterial, QSGOpaqueTextureMaterial);

  MO_ADD_METAOBJECT1(QSGVertexColorMaterial, QSGMaterial);
}

void QuickInspector::registerVariantHandlers()
{
  VariantHandler::registerStringConverter<QQuickItem::Flags>(qQuickItemFlagsToString);
  VariantHandler::registerStringConverter<QQuickPaintedItem::PerformanceHints>(qQuickPaintedItemPerformanceHintsToString);
  VariantHandler::registerStringConverter<QSGNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGBasicGeometryNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGGeometryNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGClipNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<const QSGClipNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGTransformNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGRootNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGOpacityNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGNode::Flags>(qSGNodeFlagsToString);
  VariantHandler::registerStringConverter<QSGNode::DirtyState>(qSGNodeDirtyStateToString);
  VariantHandler::registerStringConverter<const QSGClipNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGGeometry*>(Util::addressToString);
  VariantHandler::registerStringConverter<const QSGGeometry*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGMaterial*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGMaterial::Flags>(qsgMaterialFlagsToString);
  VariantHandler::registerStringConverter<QSGTexture::Filtering>(qsgTextureFilteringToString);
  VariantHandler::registerStringConverter<QSGTexture::WrapMode>(qsgTextureWrapModeToString);
}

void QuickInspector::registerPCExtensions()
{
  PropertyController::registerExtension<MaterialExtension>();
  PropertyController::registerExtension<SGGeometryExtension>();
}

#define QSG_CHECK_TYPE(Class) \
  if(dynamic_cast<Class*>(node) && MetaObjectRepository::instance()->hasMetaObject(QStringLiteral(#Class))) \
    return QStringLiteral(#Class)

QString QuickInspector::findSGNodeType(QSGNode *node) const
{
    // keep this in reverse topological order of the class hierarchy!
    QSG_CHECK_TYPE(QSGClipNode);
    QSG_CHECK_TYPE(QSGGeometryNode);
    QSG_CHECK_TYPE(QSGBasicGeometryNode);
    QSG_CHECK_TYPE(QSGTransformNode);
    QSG_CHECK_TYPE(QSGRootNode);
    QSG_CHECK_TYPE(QSGOpacityNode);

    return QStringLiteral("QSGNode");
}

QString QuickInspectorFactory::name() const
{
  return tr("Quick Scenes");
}
