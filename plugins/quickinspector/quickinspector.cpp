/*
  qmlsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickinspector.h"
#include "quickitemmodel.h"
#include "quickscenegraphmodel.h"
#include "transferimage.h"
#include "geometryextension/sggeometryextension.h"
#include "materialextension/materialextension.h"

#include <common/objectbroker.h>

#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/probeinterface.h>
#include <core/propertycontroller.h>
#include <core/remote/server.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/varianthandler.h>

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

Q_DECLARE_METATYPE(QQmlError)

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
using namespace GammaRay;

static QString qSGNodeFlagsToString(QSGNode::Flags flags)
{
  QStringList list;
  if (flags & QSGNode::OwnedByParent) {
    list << "OwnedByParent";
  }
  if (flags & QSGNode::UsePreprocess) {
    list << "UsePreprocess";
  }
  if (flags & QSGNode::OwnsGeometry) {
    list << "OwnsGeometry";
  }
  if (flags & QSGNode::OwnsMaterial) {
    list << "OwnsMaterial";
  }
  if (flags & QSGNode::OwnsOpaqueMaterial) {
    list << "OwnsOpaqueMaterial";
  }
  if (list.isEmpty())
    return "<none>";
  return list.join(" | ");
}
static QString qSGNodeDirtyStateToString(QSGNode::DirtyState flags)
{
  QStringList list;
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
  if (flags & QSGNode::DirtySubtreeBlocked) {
    list << "DirtySubtreeBlocked";
  }
#endif
  if (flags & QSGNode::DirtyMatrix) {
    list << "DirtyMatrix";
  }
  if (flags & QSGNode::DirtyNodeAdded) {
    list << "DirtyNodeAdded";
  }
  if (flags & QSGNode::DirtyNodeRemoved) {
    list << "DirtyNodeRemoved";
  }
  if (flags & QSGNode::DirtyGeometry) {
    list << "DirtyGeometry";
  }
  if (flags & QSGNode::DirtyMaterial) {
    list << "DirtyMaterial";
  }
  if (flags & QSGNode::DirtyOpacity) {
    list << "DirtyOpacity";
  }
  if (flags & QSGNode::DirtyForceUpdate) {
    list << "DirtyForceUpdate";
  }
  if (flags & QSGNode::DirtyUsePreprocess) {
    list << "DirtyUsePreprocess";
  }
  if (flags & QSGNode::DirtyPropagationMask) {
    list << "DirtyPropagationMask";
  }
  if (list.isEmpty())
    return "Clean";
  return list.join(" | ");
}

static QString qsgMaterialFlagsToString(QSGMaterial::Flags flags)
{
  QStringList list;
#define F(f) if (flags & QSGMaterial::f) list.push_back(#f);
  F(Blending)
  F(RequiresDeterminant)
  F(RequiresFullMatrixExceptTranslate)
  F(RequiresFullMatrix)
  F(CustomCompileStep)
#undef F

  if (list.isEmpty())
    return "<none>";
  return list.join(" | ");
}

static QString qsgTextureFilteringToString(QSGTexture::Filtering filtering)
{
  switch (filtering) {
    case QSGTexture::None: return "None";
    case QSGTexture::Nearest: return "Nearest";
    case QSGTexture::Linear: return "Linear";
  }
  return QString("Unknown: %1").arg(filtering);
}

static QString qsgTextureWrapModeToString(QSGTexture::WrapMode wrapMode)
{
  switch (wrapMode) {
    case QSGTexture::Repeat: return "Repeat";
    case QSGTexture::ClampToEdge: return "ClampToEdge";
  }
  return QString("Unknown: %1").arg(wrapMode);
}

QuickInspector::QuickInspector(ProbeInterface *probe, QObject *parent)
  : QuickInspectorInterface(parent),
    m_source(0),
    m_probe(probe),
    m_currentSgNode(0),
    m_itemModel(new QuickItemModel(this)),
    m_sgModel(new QuickSceneGraphModel(this)),
    m_itemPropertyController(new PropertyController("com.kdab.GammaRay.QuickItem", this)),
    m_sgPropertyController(new PropertyController("com.kdab.GammaRay.QuickSceneGraph", this)),
    m_clientViewActive(false),
    m_needsNewFrame(false)
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
  probe->registerModel("com.kdab.GammaRay.QuickWindowModel", m_windowModel);
  probe->registerModel("com.kdab.GammaRay.QuickItemModel", m_itemModel);

  connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
          m_itemModel, SLOT(objectAdded(QObject*)));
  connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)),
          m_itemModel, SLOT(objectRemoved(QObject*)));
  connect(probe->probe(), SIGNAL(objectSelected(QObject*,QPoint)),
          SLOT(objectSelected(QObject*)));
  connect(probe->probe(), SIGNAL(nonQObjectSelected(void*,QString)),
          SLOT(objectSelected(void*,QString)));

  m_itemSelectionModel = ObjectBroker::selectionModel(m_itemModel);
  connect(m_itemSelectionModel, &QItemSelectionModel::selectionChanged,
          this, &QuickInspector::itemSelectionChanged);

  probe->registerModel("com.kdab.GammaRay.QuickSceneGraphModel", m_sgModel);

  m_sgSelectionModel = ObjectBroker::selectionModel(m_sgModel);
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
    // Insert a ShaderEffectSource to the scene, with the contentItem as its source, in
    // order to use it to generate a preview of the window as QImage to show on the client.
    QQuickItem *contentItem = m_window->contentItem();
#if QT_VERSION < QT_VERSION_CHECK(5, 3, 0) // working around the 0-sized contentItem in older Qt
    if (contentItem->height() == 0 && contentItem->width() == 0) {
      contentItem->setWidth(contentItem->childrenRect().width());
      contentItem->setHeight(contentItem->childrenRect().height());
    }
#endif
    delete m_source;
    m_source = new QQuickShaderEffectSource(contentItem);
    m_source->setParent(this); // hides the item in the object tree (note: parent != parentItem)
    QQuickItemPrivate::get(m_source)->anchors()->setFill(contentItem);
    m_source->setScale(0); // The item shouldn't be visible in the original scene, but it still
                           // needs to be rendered. (i.e. setVisible(false) would cause it to
                           // not be rendered anymore)
    setupPreviewSource();
    connect(window, &QQuickWindow::afterRendering,
            this, &QuickInspector::slotSceneChanged, Qt::DirectConnection);

    m_window->update();
  }
}

void QuickInspector::setupPreviewSource()
{
  Q_ASSERT(m_window);
  QQuickItem *contentItem = m_window->contentItem();
  const QList<QQuickItem*> children = contentItem->childItems();
  if (children.size() == 2) { // prefer non-recursive shader sources, then we don't re-render all the time
    m_source->setSourceItem(children.at(children.indexOf(m_source) == 1 ? 0 : 1));
  } else {
    m_source->setRecursive(true);
    m_source->setSourceItem(contentItem);
  }
}

void QuickInspector::selectItem(QQuickItem *item)
{
  const QAbstractItemModel *model = m_itemModel;
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
  const QAbstractItemModel *model = m_sgModel;

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
  if (MetaObjectRepository::instance()->metaObject(typeName)->inherits("QSGNode")) {
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
  QVariantMap previewData;
  previewData.insert("rawImage", QVariant::fromValue(TransferImage(m_currentFrame))); // wrap to allow bypassing expensive PNG compression
  if (m_currentItem) {
    QQuickItem *parent = m_currentItem->parentItem();

    if (parent) {
      previewData.insert("itemRect",
                         m_currentItem->parentItem()->mapRectToScene(
                           QRectF(m_currentItem->x(), m_currentItem->y(),
                                  m_currentItem->width(), m_currentItem->height())));
    } else {
      previewData.insert("itemRect", QRectF(0, 0, m_currentItem->width(), m_currentItem->height()));
    }

    previewData.insert("boundingRect",
                       m_currentItem->mapRectToScene(m_currentItem->boundingRect()));
    previewData.insert("childrenRect",
                       m_currentItem->mapRectToScene(m_currentItem->childrenRect()));
    previewData.insert("transformOriginPoint",
                       m_currentItem->mapToScene(m_currentItem->transformOriginPoint()));

    QQuickAnchors *anchors = m_currentItem->property("anchors").value<QQuickAnchors*>();

    if (anchors) {
      QQuickAnchors::Anchors usedAnchors = anchors->usedAnchors();
      previewData.insert("left",
                         (bool)(usedAnchors & QQuickAnchors::LeftAnchor) || anchors->fill());
      previewData.insert("right",
                         (bool)(usedAnchors & QQuickAnchors::RightAnchor) || anchors->fill());
      previewData.insert("top",
                         (bool)(usedAnchors & QQuickAnchors::TopAnchor) || anchors->fill());
      previewData.insert("bottom",
                         (bool)(usedAnchors & QQuickAnchors::BottomAnchor) || anchors->fill());
      previewData.insert("baseline",
                         (bool)(usedAnchors & QQuickAnchors::BaselineAnchor));
      previewData.insert("horizontalCenter",
                         (bool)(usedAnchors & QQuickAnchors::HCenterAnchor) || anchors->centerIn());
      previewData.insert("verticalCenter",
                         (bool)(usedAnchors & QQuickAnchors::VCenterAnchor) || anchors->centerIn());
      previewData.insert("leftMargin", anchors->leftMargin());
      previewData.insert("rightMargin", anchors->rightMargin());
      previewData.insert("topMargin", anchors->topMargin());
      previewData.insert("bottomMargin", anchors->bottomMargin());
      previewData.insert("horizontalCenterOffset", anchors->horizontalCenterOffset());
      previewData.insert("verticalCenterOffset", anchors->verticalCenterOffset());
      previewData.insert("baselineOffset", anchors->baselineOffset());
      previewData.insert("margins", anchors->margins());
    }
    previewData.insert("x", m_currentItem->x());
    previewData.insert("y", m_currentItem->y());
    QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(m_currentItem);
    previewData.insert("transform", itemPriv->itemToWindowTransform());
    if (parent) {
      QQuickItemPrivate *parentPriv = QQuickItemPrivate::get(parent);
      previewData.insert("parentTransform", parentPriv->itemToWindowTransform());
    }
  }
  emit sceneRendered(previewData);
}

void QuickInspector::slotSceneChanged()
{
  if (!m_clientViewActive || !m_window) {
    return;
  }

  if (!m_needsNewFrame) {
    emit sceneChanged();
    return;
  }

  const QSGTextureProvider *provider = m_source->textureProvider();
  Q_ASSERT(provider);

#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
  const QQuickShaderEffectTexture *texture =
    qobject_cast<QQuickShaderEffectTexture*>(provider->texture());
  Q_ASSERT(texture);
#else
  const QSGLayer *texture = qobject_cast<QSGLayer*>(provider->texture());
  Q_ASSERT(texture);
#endif

  QOpenGLContext *ctx =
    QQuickItemPrivate::get(m_source)->sceneGraphRenderContext()->openglContext();
  if (ctx->makeCurrent(ctx->surface())) {
    m_currentFrame = texture->toImage();
  }

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
  m_window->update();
#else
  Q_UNUSED(customRenderMode);
#endif
}

void QuickInspector::checkFeatures()
{
  emit features(
    Features(
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    CustomRenderModes
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
    m_sgSelectionModel->select(m_sgModel->indexForNode(m_currentSgNode),
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
    if (m_source)
      setupPreviewSource();
    m_window->update();
  }
  if (!active && m_source)
    m_source->setSourceItem(0); // no need to render the screenshot as well if nobody is watching
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
  if(dynamic_cast<Class*>(node) && MetaObjectRepository::instance()->hasMetaObject(#Class)) \
    return QLatin1String(#Class)

QString QuickInspector::findSGNodeType(QSGNode *node) const
{
    // keep this in reverse topological order of the class hierarchy!
    QSG_CHECK_TYPE(QSGClipNode);
    QSG_CHECK_TYPE(QSGGeometryNode);
    QSG_CHECK_TYPE(QSGBasicGeometryNode);
    QSG_CHECK_TYPE(QSGTransformNode);
    QSG_CHECK_TYPE(QSGRootNode);
    QSG_CHECK_TYPE(QSGOpacityNode);

    return QLatin1String("QSGNode");
}

QString QuickInspectorFactory::name() const
{
  return tr("Quick Scenes");
}
