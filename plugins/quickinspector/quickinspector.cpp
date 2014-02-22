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
#include <QQuickView>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlError>

#include <QDebug>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QPainter>
#include <QSGNode>
#include <QSGGeometry>
#include <QSGMaterial>
#include <QMatrix4x4>

Q_DECLARE_METATYPE(QQmlError)

Q_DECLARE_METATYPE(QSGNode*)
Q_DECLARE_METATYPE(QSGBasicGeometryNode*)
Q_DECLARE_METATYPE(QSGGeometryNode*)
Q_DECLARE_METATYPE(QSGClipNode*)
Q_DECLARE_METATYPE(QSGTransformNode*)
Q_DECLARE_METATYPE(QSGRootNode*)
Q_DECLARE_METATYPE(QSGOpacityNode*)
Q_DECLARE_METATYPE(QSGNode::Flags)
Q_DECLARE_METATYPE(QSGNode::DirtyState)
Q_DECLARE_METATYPE(QSGGeometry*)
Q_DECLARE_METATYPE(QMatrix4x4*)
Q_DECLARE_METATYPE(const QMatrix4x4*)
Q_DECLARE_METATYPE(const QSGClipNode*)
Q_DECLARE_METATYPE(const QSGGeometry*)
Q_DECLARE_METATYPE(QSGMaterial*)
using namespace GammaRay;

static QString qSGNodeFlagsToString(QSGNode::Flags flags) {
    QStringList list;
    if (flags & QSGNode::OwnedByParent)
        list << "OwnedByParent";
    if (flags & QSGNode::UsePreprocess)
        list << "UsePreprocess";
    if (flags & QSGNode::OwnsGeometry)
        list << "OwnsGeometry";
    if (flags & QSGNode::OwnsMaterial)
        list << "OwnsMaterial";
    if (flags & QSGNode::OwnsOpaqueMaterial)
        list << "OwnsOpaqueMaterial";
    return list.join(" | ");
}
static QString qSGNodeDirtyStateToString(QSGNode::DirtyState flags) {
    QStringList list;
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    if (flags & QSGNode::DirtySubtreeBlocked)
        list << "DirtySubtreeBlocked";
#endif
    if (flags & QSGNode::DirtyMatrix)
        list << "DirtyMatrix";
    if (flags & QSGNode::DirtyNodeAdded)
        list << "DirtyNodeAdded";
    if (flags & QSGNode::DirtyNodeRemoved)
        list << "DirtyNodeRemoved";
    if (flags & QSGNode::DirtyGeometry)
        list << "DirtyGeometry";
    if (flags & QSGNode::DirtyMaterial)
        list << "DirtyMaterial";
    if (flags & QSGNode::DirtyOpacity)
        list << "DirtyOpacity";
    if (flags & QSGNode::DirtyForceUpdate)
        list << "DirtyForceUpdate";
    if (flags & QSGNode::DirtyUsePreprocess)
        list << "DirtyUsePreprocess";
    if (flags & QSGNode::DirtyPropagationMask)
        list << "DirtyPropagationMask";
    return list.join(" | ");
}


QuickInspector::QuickInspector(ProbeInterface* probe, QObject* parent) :
  QuickInspectorInterface(parent),
  m_probe(probe),
  m_itemModel(new QuickItemModel(this)),
  m_itemPropertyController(new PropertyController("com.kdab.GammaRay.QuickItem", this)),
#ifdef HAVE_SG_INSPECTOR
  m_sgModel(new QuickSceneGraphModel(this)),
  m_sgPropertyController(new PropertyController("com.kdab.GammaRay.QuickSceneGraph", this)),
#endif
  m_clientConnected(false)
{
  registerPCExtensions();
  Server::instance()->registerMonitorNotifier(Endpoint::instance()->objectAddress(objectName()), this, "clientConnectedChanged");

  registerMetaTypes();
  registerVariantHandlers();
  probe->installGlobalEventFilter(this);

  QAbstractProxyModel* windowModel = new ObjectTypeFilterProxyModel<QQuickWindow>(this);
  windowModel->setSourceModel(probe->objectListModel());
  QAbstractProxyModel* proxy = new SingleColumnObjectProxyModel(this);
  proxy->setSourceModel(windowModel);
  m_windowModel = proxy;
  probe->registerModel("com.kdab.GammaRay.QuickWindowModel", m_windowModel);
  probe->registerModel("com.kdab.GammaRay.QuickItemModel", m_itemModel);

  connect(probe->probe(), SIGNAL(objectCreated(QObject*)), m_itemModel, SLOT(objectAdded(QObject*)));
  connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)), m_itemModel, SLOT(objectRemoved(QObject*)));

  m_itemSelectionModel = ObjectBroker::selectionModel(m_itemModel);
  connect(m_itemSelectionModel, &QItemSelectionModel::selectionChanged, this, &QuickInspector::itemSelectionChanged);

#ifdef HAVE_SG_INSPECTOR
  probe->registerModel("com.kdab.GammaRay.QuickSceneGraphModel", m_sgModel);

  m_sgSelectionModel = ObjectBroker::selectionModel(m_sgModel);
  connect(m_sgSelectionModel, &QItemSelectionModel::selectionChanged, this, &QuickInspector::sgSelectionChanged);
  connect(m_sgModel, &QuickSceneGraphModel::nodeDeleted, this, &QuickInspector::sgNodeDeleted);
#endif
}

QuickInspector::~QuickInspector()
{
}

void QuickInspector::selectWindow(int index)
{
  const QModelIndex mi = m_windowModel->index(index, 0);
  QQuickWindow* window = mi.data(ObjectModel::ObjectRole).value<QQuickWindow*>();
  selectWindow(window);
}

void QuickInspector::selectWindow(QQuickWindow* window)
{
  if (m_window) {
    disconnect(m_window, 0, this, 0);
  }

  m_window = window;
  m_itemModel->setWindow(window);
#ifdef HAVE_SG_INSPECTOR
  m_sgModel->setWindow(window);
#endif

  if (m_window) {
    connect(window, &QQuickWindow::frameSwapped, this, &QuickInspector::emitSceneChanged);
  }

  emitSceneChanged();
}

void QuickInspector::selectItem(QQuickItem* item)
{
  const QAbstractItemModel *model = m_itemModel;
  const QModelIndexList indexList = model->match(model->index(0, 0), ObjectModel::ObjectRole,
    QVariant::fromValue<QQuickItem*>(item), 1, Qt::MatchExactly | Qt::MatchRecursive);
  if (indexList.isEmpty())
    return;

  const QModelIndex index = indexList.first();
  m_itemSelectionModel->select( index, QItemSelectionModel::Select | QItemSelectionModel::Clear |
    QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

void QuickInspector::renderScene()
{
  if (!m_clientConnected || !m_window)
    return;

  QImage img = m_window->grabWindow();
  if (m_currentItem) {
    QPainter p(&img);

    // bounding box
    const QRectF itemRect(0, 0, m_currentItem->width(), m_currentItem->height());
    p.setPen(Qt::red);
    p.drawRect(m_currentItem->mapRectToScene(itemRect));

    // children rect
    p.setPen(Qt::cyan);
    p.drawRect(m_currentItem->mapRectToScene(m_currentItem->childrenRect()));

    // transform origin
    p.setPen(Qt::red);
    const QPointF tfo = m_currentItem->property("transformOriginPoint").toPointF();
    p.drawEllipse(m_currentItem->mapToScene(tfo), 2, 2);
  }

  emit sceneRendered(img);
}

void QuickInspector::emitSceneChanged()
{
  if (m_clientConnected && m_window)
    emit sceneChanged();
}

void QuickInspector::itemSelectionChanged(const QItemSelection& selection)
{
  if (selection.isEmpty())
    return;
  const QModelIndex index = selection.first().topLeft();
  m_currentItem = index.data(ObjectModel::ObjectRole).value<QQuickItem*>();
  m_itemPropertyController->setObject(m_currentItem);

#ifdef HAVE_SG_INSPECTOR
  // It might be that a sg-node is already selected that belongs to this item, but isn't the root
  // node of the Item. In this case we don't want to overwrite that selection.
  if (m_sgModel->itemForSgNode(m_currentSgNode) != m_currentItem) {
    m_currentSgNode = m_sgModel->sgNodeForItem(m_currentItem);
    m_sgSelectionModel->select(m_sgModel->indexForNode(m_currentSgNode), QItemSelectionModel::Select |
      QItemSelectionModel::Clear | QItemSelectionModel::Rows | QItemSelectionModel::Current);
  }
#endif

  emitSceneChanged();
}

void QuickInspector::sgSelectionChanged(const QItemSelection& selection)
{
  if (selection.isEmpty())
    return;
#ifdef HAVE_SG_INSPECTOR
  const QModelIndex index = selection.first().topLeft();
  m_currentSgNode = index.data(ObjectModel::ObjectRole).value<QSGNode*>();
  m_sgPropertyController->setObject(m_currentSgNode, findSGNodeType(m_currentSgNode));

  m_currentItem = m_sgModel->itemForSgNode(m_currentSgNode);
  selectItem(m_currentItem);
#endif
}

void QuickInspector::sgNodeDeleted(QSGNode *node)
{
#ifdef HAVE_SG_INSPECTOR
  if (m_currentSgNode == node)
    m_sgPropertyController->setObject(0);
#else
  Q_UNUSED(node);
#endif
}

void QuickInspector::clientConnectedChanged(bool connected)
{
  m_clientConnected = connected;
  emitSceneChanged();
}

QQuickItem* QuickInspector::recursiveChiltAt(QQuickItem* parent, const QPointF& pos) const
{
  Q_ASSERT(parent);
  QQuickItem *child = parent->childAt(pos.x(), pos.y());
  if (child)
    return recursiveChiltAt(child, parent->mapToItem(child, pos));
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
        selectItem(item);
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
  MO_ADD_PROPERTY_RO(QQuickWindow, QQuickItem*, mouseGrabberItem);
  MO_ADD_PROPERTY_RO(QQuickWindow, QOpenGLContext*, openglContext);
  MO_ADD_PROPERTY_RO(QQuickWindow, uint, renderTargetId);

  MO_ADD_METAOBJECT1(QQuickView, QQuickWindow);
  MO_ADD_PROPERTY_RO(QQuickView, QQmlEngine*, engine);
  MO_ADD_PROPERTY_RO(QQuickView, QList<QQmlError>, errors);
  MO_ADD_PROPERTY_RO(QQuickView, QSize, initialSize);
  MO_ADD_PROPERTY_RO(QQuickView, QQmlContext*, rootContext);
  MO_ADD_PROPERTY_RO(QQuickView, QQuickItem*, rootObject);

  MO_ADD_METAOBJECT0(QSGNode);
  MO_ADD_PROPERTY_RO(QSGNode, QSGNode*, parent);
  MO_ADD_PROPERTY_RO(QSGNode, int, childCount);
  MO_ADD_PROPERTY_RO(QSGNode, QSGNode::Flags, flags);
  MO_ADD_PROPERTY   (QSGNode, QSGNode::DirtyState, dirtyState, markDirty);

  MO_ADD_METAOBJECT1(QSGBasicGeometryNode, QSGNode);
  MO_ADD_PROPERTY_RO(QSGBasicGeometryNode, const QSGGeometry*, geometry);
  MO_ADD_PROPERTY_RO(QSGBasicGeometryNode, const QMatrix4x4*, matrix);
  MO_ADD_PROPERTY_RO(QSGBasicGeometryNode, const QSGClipNode*, clipList);

  MO_ADD_METAOBJECT1(QSGGeometryNode, QSGBasicGeometryNode);
  MO_ADD_PROPERTY   (QSGGeometryNode, QSGMaterial*, material, setMaterial);
  MO_ADD_PROPERTY   (QSGGeometryNode, QSGMaterial*, opaqueMaterial, setOpaqueMaterial);
  MO_ADD_PROPERTY_RO(QSGGeometryNode, QSGMaterial*, activeMaterial);
  MO_ADD_PROPERTY   (QSGGeometryNode, int, renderOrder, setRenderOrder);
  MO_ADD_PROPERTY   (QSGGeometryNode, qreal, inheritedOpacity, setInheritedOpacity);

  MO_ADD_METAOBJECT1(QSGClipNode, QSGBasicGeometryNode);
  MO_ADD_PROPERTY   (QSGClipNode, bool, isRectangular, setIsRectangular);
  MO_ADD_PROPERTY_CR(QSGClipNode, QRectF, clipRect, setClipRect);
  MO_ADD_PROPERTY_RO(QSGClipNode, const QMatrix4x4*, matrix);
  MO_ADD_PROPERTY_RO(QSGClipNode, const QSGClipNode*, clipList);

  MO_ADD_METAOBJECT1(QSGTransformNode, QSGNode);
//  MO_ADD_PROPERTY   (QSGTransformNode, const QMatrix4x4&, matrix, setMatrix);
//  MO_ADD_PROPERTY   (QSGTransformNode, const QMatrix4x4&, combinedMatrix, setCombinedMatrix);

  MO_ADD_METAOBJECT1(QSGRootNode, QSGNode);

  MO_ADD_METAOBJECT1(QSGOpacityNode, QSGNode);
  MO_ADD_PROPERTY   (QSGOpacityNode, qreal, opacity, setOpacity);
  MO_ADD_PROPERTY   (QSGOpacityNode, qreal, combinedOpacity, setCombinedOpacity);
  MO_ADD_PROPERTY_RO(QSGOpacityNode, bool, isSubtreeBlocked);
}

void QuickInspector::registerVariantHandlers()
{
  VariantHandler::registerStringConverter<QSGNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGBasicGeometryNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGGeometryNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGClipNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGTransformNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGRootNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGOpacityNode*>(Util::addressToString);
  VariantHandler::registerStringConverter<QSGNode::Flags>(qSGNodeFlagsToString);
  VariantHandler::registerStringConverter<QSGNode::DirtyState>(qSGNodeDirtyStateToString);
}

void QuickInspector::registerPCExtensions()
{
  PropertyController::registerExtension<MaterialExtension>();
  PropertyController::registerExtension<SGGeometryExtension>();
}

#define QSG_CHECK_TYPE(Class) \
  if (dynamic_cast<Class*>(node) && MetaObjectRepository::instance()->hasMetaObject(#Class)) \
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
