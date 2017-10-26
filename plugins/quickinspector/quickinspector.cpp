/*
  qmlsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "quickoverlay.h"
#include "quickitemmodel.h"
#include "quickscenegraphmodel.h"
#include "quickpaintanalyzerextension.h"
#include "quickimplicitbindingdependencyprovider.h"
#include "geometryextension/sggeometryextension.h"
#include "materialextension/materialextension.h"
#include "materialextension/qquickopenglshadereffectmaterialadaptor.h"
#include "textureextension/qsgtexturegrabber.h"
#include "textureextension/textureextension.h"

#include <common/modelevent.h>
#include <common/objectbroker.h>
#include <common/probecontrollerinterface.h>
#include <common/remoteviewframe.h>

#include <core/metaenum.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/probeguard.h>
#include <core/probeinterface.h>
#include <core/propertycontroller.h>
#include <core/remote/server.h>
#include <core/remote/serverproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/varianthandler.h>
#include <core/remoteviewserver.h>
#include <core/tools/bindinginspector/qmlbindingmodel.h>

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
#include <QSGNode>
#include <QSGGeometry>
#include <QSGMaterial>
#include <QSGFlatColorMaterial>
#include <QSGTextureMaterial>
#include <QSGVertexColorMaterial>
#include <private/qquickshadereffectsource_p.h>
#include <QMatrix4x4>
#include <QCoreApplication>

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
#include <QSGRenderNode>
#include <QSGRendererInterface>
#include <private/qquickopenglshadereffectnode_p.h>
#endif

#include <private/qquickanchors_p.h>
#include <private/qquickitem_p.h>
#include <private/qsgbatchrenderer_p.h>
#include <private/qsgdistancefieldglyphnode_p_p.h>

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
Q_DECLARE_METATYPE(QSGRenderNode *)
Q_DECLARE_METATYPE(QSGRenderNode::RenderingFlags)
Q_DECLARE_METATYPE(QSGRenderNode::StateFlags)
#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(Qt::MouseButtons)
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
Q_DECLARE_METATYPE(QSGRendererInterface*)
Q_DECLARE_METATYPE(QSGRendererInterface::GraphicsApi)
Q_DECLARE_METATYPE(QSGRendererInterface::ShaderCompilationTypes)
Q_DECLARE_METATYPE(QSGRendererInterface::ShaderSourceTypes)
Q_DECLARE_METATYPE(QSGRendererInterface::ShaderType)
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
    if (flags & QSGNode::OwnedByParent)
        list << QStringLiteral("OwnedByParent");
    if (flags & QSGNode::UsePreprocess)
        list << QStringLiteral("UsePreprocess");
    if (flags & QSGNode::OwnsGeometry)
        list << QStringLiteral("OwnsGeometry");
    if (flags & QSGNode::OwnsMaterial)
        list << QStringLiteral("OwnsMaterial");
    if (flags & QSGNode::OwnsOpaqueMaterial)
        list << QStringLiteral("OwnsOpaqueMaterial");
    if (list.isEmpty())
        return QStringLiteral("<none>");
    return list.join(QStringLiteral(" | "));
}

static QString qSGNodeDirtyStateToString(QSGNode::DirtyState flags)
{
    QStringList list;
    if (flags & QSGNode::DirtySubtreeBlocked)
        list << QStringLiteral("DirtySubtreeBlocked");
    if (flags & QSGNode::DirtyMatrix)
        list << QStringLiteral("DirtyMatrix");
    if (flags & QSGNode::DirtyNodeAdded)
        list << QStringLiteral("DirtyNodeAdded");
    if (flags & QSGNode::DirtyNodeRemoved)
        list << QStringLiteral("DirtyNodeRemoved");
    if (flags & QSGNode::DirtyGeometry)
        list << QStringLiteral("DirtyGeometry");
    if (flags & QSGNode::DirtyMaterial)
        list << QStringLiteral("DirtyMaterial");
    if (flags & QSGNode::DirtyOpacity)
        list << QStringLiteral("DirtyOpacity");
    if (flags & QSGNode::DirtyForceUpdate)
        list << QStringLiteral("DirtyForceUpdate");
    if (flags & QSGNode::DirtyUsePreprocess)
        list << QStringLiteral("DirtyUsePreprocess");
    if (flags & QSGNode::DirtyPropagationMask)
        list << QStringLiteral("DirtyPropagationMask");
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
    case QSGTexture::None:
        return QStringLiteral("None");
    case QSGTexture::Nearest:
        return QStringLiteral("Nearest");
    case QSGTexture::Linear:
        return QStringLiteral("Linear");
    }
    return QStringLiteral("Unknown: %1").arg(filtering);
}

static QString qsgTextureWrapModeToString(QSGTexture::WrapMode wrapMode)
{
    switch (wrapMode) {
    case QSGTexture::Repeat:
        return QStringLiteral("Repeat");
    case QSGTexture::ClampToEdge:
        return QStringLiteral("ClampToEdge");
    }
    return QStringLiteral("Unknown: %1").arg(wrapMode);
}

static bool isGoodCandidateItem(QQuickItem *item)
{

    if (!item->isVisible() || qFuzzyCompare(item->opacity() + 1.0, qreal(1.0)) ||
            !item->flags().testFlag(QQuickItem::ItemHasContents)) {
        return false;
    }

    return true;
}

static QByteArray renderModeToString(QuickInspectorInterface::RenderMode customRenderMode)
{
    switch (customRenderMode) {
        case QuickInspectorInterface::VisualizeClipping:
            return QByteArray("clip");
        case QuickInspectorInterface::VisualizeOverdraw:
            return QByteArray("overdraw");
        case QuickInspectorInterface::VisualizeBatches:
            return QByteArray("batches");
        case QuickInspectorInterface::VisualizeChanges:
            return QByteArray("changes");
        case QuickInspectorInterface::VisualizeTraces:
        case QuickInspectorInterface::NormalRendering:
            break;
    }
    return QByteArray();
}

QMutex RenderModeRequest::mutex;

RenderModeRequest::RenderModeRequest(QObject *parent)
    : QObject(parent)
    , mode(QuickInspectorInterface::NormalRendering)
{
}

RenderModeRequest::~RenderModeRequest()
{
    QMutexLocker lock(&mutex);

    window.clear();

    if (connection)
        disconnect(connection);
}

void RenderModeRequest::applyOrDelay(QQuickWindow *toWindow, QuickInspectorInterface::RenderMode customRenderMode)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    if (toWindow) {
        QMutexLocker lock(&mutex);
# if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        // Qt does some performance optimizations that break custom render modes.
        // Thus the optimizations are only applied if there is no custom render mode set.
        // So we need to make the scenegraph recheck whether a custom render mode is set.
        // We do this by simply cleaning the scene graph which will recreate the renderer.
        // We need however to do that at the proper time from the gui thread.

        if (!connection ||
                (mode != customRenderMode || window != toWindow)) {
            if (connection)
                disconnect(connection);
            mode = customRenderMode;
            window = toWindow;
            connection = connect(window.data(), &QQuickWindow::afterRendering, this, &RenderModeRequest::apply, Qt::DirectConnection);
            // trigger window update so afterRendering is emitted
            QMetaObject::invokeMethod(window, "update", Qt::QueuedConnection);
        }
# else
        if (window != toWindow || mode != customRenderMode) {
            window = toWindow;
            mode = customRenderMode;
            QMetaObject::invokeMethod(this, "apply", Qt::QueuedConnection);
        } else {
            QMetaObject::invokeMethod(this, "preFinished", Qt::QueuedConnection);
        }
# endif
    }
#else
    Q_UNUSED(toWindow);
    Q_UNUSED(customRenderMode);
#endif
}

void RenderModeRequest::apply()
{
    QMutexLocker lock(&mutex);

    if (connection)
        disconnect(connection);

    if (window) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        emit aboutToCleanSceneGraph();
        const QByteArray mode = renderModeToString(RenderModeRequest::mode);
        QQuickWindowPrivate *winPriv = QQuickWindowPrivate::get(window);
        QMetaObject::invokeMethod(window, "cleanupSceneGraph", Qt::DirectConnection);
        winPriv->customRenderMode = mode;
        emit sceneGraphCleanedUp();
#endif
    }

    QMetaObject::invokeMethod(this, "preFinished", Qt::QueuedConnection);
}

void RenderModeRequest::preFinished()
{
    QMutexLocker lock(&mutex);

    if (window)
        window->update();

    emit finished();
}

QuickInspector::QuickInspector(ProbeInterface *probe, QObject *parent)
    : QuickInspectorInterface(parent)
    , m_probe(probe)
    , m_currentSgNode(nullptr)
    , m_itemModel(new QuickItemModel(this))
    , m_sgModel(new QuickSceneGraphModel(this))
    , m_itemPropertyController(new PropertyController(QStringLiteral("com.kdab.GammaRay.QuickItem"),
                                                      this))
    , m_sgPropertyController(new PropertyController(QStringLiteral(
                                                        "com.kdab.GammaRay.QuickSceneGraph"), this))
    , m_remoteView(new RemoteViewServer(QStringLiteral("com.kdab.GammaRay.QuickRemoteView"), this))
    , m_pendingRenderMode(new RenderModeRequest(this))
    , m_renderMode(QuickInspectorInterface::NormalRendering)
{
    registerMetaTypes();
    registerVariantHandlers();
    probe->installGlobalEventFilter(this);

    recreateOverlay();

    QAbstractProxyModel *windowModel = new ObjectTypeFilterProxyModel<QQuickWindow>(this);
    windowModel->setSourceModel(probe->objectListModel());
    QAbstractProxyModel *proxy = new SingleColumnObjectProxyModel(this);
    proxy->setSourceModel(windowModel);
    m_windowModel = proxy;
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.QuickWindowModel"), m_windowModel);

    auto filterProxy = new ServerProxyModel<KRecursiveFilterProxyModel>(this);
    filterProxy->setSourceModel(m_itemModel);
    filterProxy->addRole(ObjectModel::ObjectIdRole);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.QuickItemModel"), filterProxy);

    if (m_probe->needsObjectDiscovery()) {
        connect(m_probe->probe(), SIGNAL(objectCreated(QObject*)),
                SLOT(objectCreated(QObject*)));
    }

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

    connect(m_remoteView, &RemoteViewServer::elementsAtRequested, this, &QuickInspector::requestElementsAt);
    connect(this, &QuickInspector::elementsAtReceived, m_remoteView, &RemoteViewServer::elementsAtReceived);
    connect(m_remoteView, &RemoteViewServer::doPickElementId, this, &QuickInspector::pickElementId);
    connect(m_remoteView, &RemoteViewServer::requestUpdate, this, &QuickInspector::slotGrabWindow);
    connect(m_pendingRenderMode, &RenderModeRequest::aboutToCleanSceneGraph, this, &QuickInspector::aboutToCleanSceneGraph);
    connect(m_pendingRenderMode, &RenderModeRequest::sceneGraphCleanedUp, this, &QuickInspector::sceneGraphCleanedUp);

    auto texGrab = new QSGTextureGrabber(this);
    connect(probe->probe(), SIGNAL(objectCreated(QObject*)), texGrab, SLOT(objectCreated(QObject*)));

    // needs to be last, extensions require some of the above to be set up correctly
    registerPCExtensions();
}

QuickInspector::~QuickInspector()
{
    if (m_overlay) {
        disconnect(m_overlay.data(), &QObject::destroyed, this, &QuickInspector::recreateOverlay);
        delete m_overlay.data();
    }
}

void QuickInspector::selectWindow(int index)
{
    const QModelIndex mi = m_windowModel->index(index, 0);
    QQuickWindow *window = mi.data(ObjectModel::ObjectRole).value<QQuickWindow *>();
    selectWindow(window);
}

void QuickInspector::selectWindow(QQuickWindow *window)
{
    if (m_window == window) {
        return;
    }

    if (m_window) {
        const QByteArray mode = QQuickWindowPrivate::get(m_window)->customRenderMode;

        if (!mode.isEmpty()) {
            auto reset = new RenderModeRequest(m_window);
            connect(reset, &RenderModeRequest::finished, reset, &RenderModeRequest::deleteLater);
            reset->applyOrDelay(m_window, QuickInspectorInterface::NormalRendering);
        }
    }

    m_window = window;
    m_itemModel->setWindow(window);
    m_sgModel->setWindow(window);
    m_remoteView->setEventReceiver(m_window);
    m_remoteView->resetView();
    m_overlay->setWindow(m_window);

    if (m_window) {
        // make sure we have selected something for the property editor to not be entirely empty
        selectItem(m_window->contentItem());
        m_window->update();
    }

    checkFeatures();

    if (m_window)
        setCustomRenderMode(m_renderMode);
}

void QuickInspector::selectItem(QQuickItem *item)
{
    const QAbstractItemModel *model = m_itemSelectionModel->model();
    Model::used(model);
    Model::used(m_sgSelectionModel->model());

    const QModelIndexList indexList
        = model->match(model->index(0, 0),
                       ObjectModel::ObjectRole,
                       QVariant::fromValue<QQuickItem *>(item), 1,
                       Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const QModelIndex index = indexList.first();
    m_itemSelectionModel->select(index,
                                 QItemSelectionModel::Select
                                 |QItemSelectionModel::Clear
                                 |QItemSelectionModel::Rows
                                 |QItemSelectionModel::Current);
}

void QuickInspector::selectSGNode(QSGNode *node)
{
    const QAbstractItemModel *model = m_sgSelectionModel->model();
    Model::used(model);

    const QModelIndexList indexList = model->match(model->index(0, 0), ObjectModel::ObjectRole,
                                                   QVariant::fromValue(
                                                       node), 1,
                                                   Qt::MatchExactly | Qt::MatchRecursive
                                                   | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const QModelIndex index = indexList.first();
    m_sgSelectionModel->select(index,
                               QItemSelectionModel::Select
                               |QItemSelectionModel::Clear
                               |QItemSelectionModel::Rows
                               |QItemSelectionModel::Current);
}

void QuickInspector::objectSelected(QObject *object)
{
    if (auto item = qobject_cast<QQuickItem *>(object))
        selectItem(item);
    else if (auto window = qobject_cast<QQuickWindow *>(object))
        selectWindow(window);
}

void QuickInspector::objectSelected(void *object, const QString &typeName)
{
    auto metaObject = MetaObjectRepository::instance()->metaObject(typeName);
    if (metaObject && metaObject->inherits(QStringLiteral("QSGNode")))
        selectSGNode(reinterpret_cast<QSGNode *>(object));
}

void QuickInspector::objectCreated(QObject *object)
{
    if (QQuickWindow *window = qobject_cast<QQuickWindow *>(object)) {
        if (QQuickView *view = qobject_cast<QQuickView *>(object)) {
            m_probe->discoverObject(view->engine());
        }
        else {
            QQmlContext *context = QQmlEngine::contextForObject(window);
            QQmlEngine *engine = context ? context->engine() : nullptr;

            if (!engine) {
                engine = qmlEngine(window->contentItem()->childItems().value(0));
            }

            m_probe->discoverObject(engine);
        }
    }
}

void QuickInspector::recreateOverlay()
{
    ProbeGuard guard;
    m_overlay = new QuickOverlay;

    connect(m_overlay.data(), &QuickOverlay::grabberReadyChanged, m_remoteView, &RemoteViewServer::setGrabberReady);
    connect(m_overlay.data(), &QuickOverlay::sceneChanged, m_remoteView, &RemoteViewServer::sourceChanged);
    connect(m_overlay.data(), &QuickOverlay::sceneGrabbed, this, &QuickInspector::sendRenderedScene);
    // the target application might have destroyed the overlay widget
    // (e.g. because the parent of the overlay got destroyed).
    // just recreate a new one in this case
    connect(m_overlay.data(), &QObject::destroyed, this, &QuickInspector::recreateOverlay);

    m_remoteView->setGrabberReady(true);
}

void QuickInspector::aboutToCleanSceneGraph()
{
    m_sgModel->setWindow(nullptr);
    m_currentSgNode = nullptr;
    m_sgPropertyController->setObject(nullptr, QString());
}

void QuickInspector::sceneGraphCleanedUp()
{
    m_sgModel->setWindow(m_window);
}

void QuickInspector::sendRenderedScene(const GammaRay::GrabbedFrame &grabbedFrame)
{
    RemoteViewFrame frame;
    frame.setImage(grabbedFrame.image, grabbedFrame.transform);
    frame.setSceneRect(grabbedFrame.itemsGeometryRect);
    frame.setViewRect(QRect(0, 0, m_window->width(), m_window->height()));
    if (m_overlay->settings().componentsTraces)
        frame.setData(QVariant::fromValue(grabbedFrame.itemsGeometry));
    else if (!grabbedFrame.itemsGeometry.isEmpty())
        frame.setData(QVariant::fromValue(grabbedFrame.itemsGeometry.at(0)));
    m_remoteView->sendFrame(frame);
}

void QuickInspector::slotGrabWindow()
{
    if (!m_remoteView->isActive() || !m_window)
        return;

    Q_ASSERT(QThread::currentThread() == QCoreApplication::instance()->thread());
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    if (m_window->rendererInterface()->graphicsApi() != QSGRendererInterface::OpenGL) {
        qreal dpr = 1.0;
        // See QTBUG-53795
    #if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
        dpr = m_window->effectiveDevicePixelRatio();
    #elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        dpr = m_window->devicePixelRatio();
    #endif
        GrabbedFrame grabbedFrame;
        grabbedFrame.image = m_window->grabWindow();
        grabbedFrame.image.setDevicePixelRatio(dpr);
        sendRenderedScene(grabbedFrame);
        return;
    }
#endif
    m_overlay->requestGrabWindow(m_remoteView->userViewport());
}

void QuickInspector::setCustomRenderMode(
    GammaRay::QuickInspectorInterface::RenderMode customRenderMode)
{
    m_renderMode = customRenderMode;

    m_pendingRenderMode->applyOrDelay(m_window, customRenderMode);

    const bool tracing = customRenderMode == QuickInspectorInterface::VisualizeTraces;
    if (m_overlay->settings().componentsTraces != tracing) {
        auto settings = m_overlay->settings();
        settings.componentsTraces = tracing;
        setOverlaySettings(settings);
    }
}

void QuickInspector::setServerSideDecorationsEnabled(bool enabled)
{
    m_overlay->setDecorationsEnabled(enabled);
}

void QuickInspector::checkFeatures()
{
    Features f;
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    if (m_window
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
        && m_window->rendererInterface()->graphicsApi() == QSGRendererInterface::OpenGL
#endif
    ) f = AllCustomRenderModes;
#endif
    emit features(f);
}

void QuickInspector::checkServerSideDecorations()
{
    emit serverSideDecorations(m_overlay->decorationsEnabled());
}

void QuickInspector::setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings)
{
    m_overlay->setSettings(settings);
    emit overlaySettings(m_overlay->settings());
}

void QuickInspector::checkOverlaySettings()
{
    emit overlaySettings(m_overlay->settings());
}

void QuickInspector::itemSelectionChanged(const QItemSelection &selection)
{
    const QModelIndex index = selection.value(0).topLeft();
    m_currentItem = index.data(ObjectModel::ObjectRole).value<QQuickItem *>();
    m_itemPropertyController->setObject(m_currentItem);

    // It might be that a sg-node is already selected that belongs to this item, but isn't the root
    // node of the Item. In this case we don't want to overwrite that selection.
    if (m_sgModel->itemForSgNode(m_currentSgNode) != m_currentItem) {
        m_currentSgNode = m_sgModel->sgNodeForItem(m_currentItem);
        const auto sourceIdx = m_sgModel->indexForNode(m_currentSgNode);
        auto proxy = qobject_cast<const QAbstractProxyModel *>(m_sgSelectionModel->model());
        m_sgSelectionModel->select(proxy->mapFromSource(sourceIdx),
                                   QItemSelectionModel::Select
                                   |QItemSelectionModel::Clear
                                   |QItemSelectionModel::Rows
                                   |QItemSelectionModel::Current);
    }

    m_overlay->placeOn(m_currentItem.data());
}

void QuickInspector::sgSelectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;

    const QModelIndex index = selection.first().topLeft();
    m_currentSgNode = index.data(ObjectModel::ObjectRole).value<QSGNode *>();
    if (!m_sgModel->verifyNodeValidity(m_currentSgNode))
        return; // Apparently the node has been deleted meanwhile, so don't access it.

    m_sgPropertyController->setObject(m_currentSgNode, findSGNodeType(m_currentSgNode));

    m_currentItem = m_sgModel->itemForSgNode(m_currentSgNode);
    selectItem(m_currentItem);
}

void QuickInspector::sgNodeDeleted(QSGNode *node)
{
    if (m_currentSgNode == node)
        m_sgPropertyController->setObject(nullptr, QString());
}

void QuickInspector::requestElementsAt(const QPoint &pos, GammaRay::RemoteViewInterface::RequestMode mode)
{
    if (!m_window)
        return;

    int bestCandidate;
    const ObjectIds objects = recursiveItemsAt(m_window->contentItem(), pos, mode, bestCandidate);

    if (!objects.isEmpty()) {
        emit elementsAtReceived(objects, bestCandidate);
    }
}

void QuickInspector::pickElementId(const GammaRay::ObjectId &id)
{
    QQuickItem *item = id.asQObjectType<QQuickItem *>();
    if (item)
        m_probe->selectObject(item);
}

ObjectIds QuickInspector::recursiveItemsAt(QQuickItem *parent, const QPointF &pos,
                                           GammaRay::RemoteViewInterface::RequestMode mode, int &bestCandidate) const
{
    Q_ASSERT(parent);
    ObjectIds objects;

    bestCandidate = -1;

    auto childItems = parent->childItems();
    std::stable_sort(childItems.begin(), childItems.end(),
                     [](QQuickItem *lhs, QQuickItem *rhs){return lhs->z() < rhs->z();}
    );

    for (int i = childItems.size() - 1; i >= 0; --i) { // backwards to match z order
        auto child = childItems.at(i);
        const QPointF requestedPoint = parent->mapToItem(child, pos);
        if (child->contains(requestedPoint)) {
            const bool hasSubChildren = !child->childItems().isEmpty();

            if (hasSubChildren) {
                const int count = objects.count();
                int bc; // possibly better candidate among subChildren
                objects << recursiveItemsAt(child, requestedPoint, mode, bc);

                if (bestCandidate == -1 && bc != -1) {
                    bestCandidate = count + bc;
                }
            }
            else {
                if (bestCandidate == -1 && isGoodCandidateItem(child)) {
                    bestCandidate = objects.count();
                }

                objects << ObjectId(child);
            }
        }

        if (bestCandidate != -1 && mode == RemoteViewInterface::RequestBest) {
            break;
        }
    }

    if (bestCandidate == -1 && isGoodCandidateItem(parent)) {
        bestCandidate = objects.count();
    }

    objects << ObjectId(parent);

    if (bestCandidate != -1 && mode == RemoteViewInterface::RequestBest) {
        objects = ObjectIds() << objects[bestCandidate];
        bestCandidate = 0;
    }

    return objects;
}

bool QuickInspector::eventFilter(QObject *receiver, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEv = static_cast<QMouseEvent*>(event);
        if (mouseEv->button() == Qt::LeftButton &&
                mouseEv->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
            QQuickWindow *window = qobject_cast<QQuickWindow*>(receiver);
            if (window && window->contentItem()) {
                int bestCandidate;
                const ObjectIds objects = recursiveItemsAt(window->contentItem(), mouseEv->pos(),
                                                           RemoteViewInterface::RequestBest, bestCandidate);
                m_probe->selectObject(objects.value(bestCandidate == -1 ? 0 : bestCandidate).asQObject());
            }
        }
    }

    return QObject::eventFilter(receiver, event);
}

void QuickInspector::registerMetaTypes()
{
    MetaObject *mo = nullptr;
    MO_ADD_METAOBJECT1(QQuickWindow, QWindow);
    MO_ADD_PROPERTY(QQuickWindow, clearBeforeRendering, setClearBeforeRendering);
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    MO_ADD_PROPERTY_RO(QQuickWindow, effectiveDevicePixelRatio);
#endif
    MO_ADD_PROPERTY(QQuickWindow, isPersistentOpenGLContext, setPersistentOpenGLContext);
    MO_ADD_PROPERTY(QQuickWindow, isPersistentSceneGraph, setPersistentSceneGraph);
    MO_ADD_PROPERTY_RO(QQuickWindow, mouseGrabberItem);
    MO_ADD_PROPERTY_RO(QQuickWindow, openglContext);
    MO_ADD_PROPERTY_RO(QQuickWindow, renderTargetId);
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    MO_ADD_PROPERTY_RO(QQuickWindow, rendererInterface);

    MO_ADD_METAOBJECT0(QSGRendererInterface);
    MO_ADD_PROPERTY_RO(QSGRendererInterface, graphicsApi);
    MO_ADD_PROPERTY_RO(QSGRendererInterface, shaderCompilationType);
    MO_ADD_PROPERTY_RO(QSGRendererInterface, shaderSourceType);
    MO_ADD_PROPERTY_RO(QSGRendererInterface, shaderType);
#endif

    MO_ADD_METAOBJECT1(QQuickView, QQuickWindow);
    MO_ADD_PROPERTY_RO(QQuickView, engine);
    MO_ADD_PROPERTY_RO(QQuickView, errors);
    MO_ADD_PROPERTY_RO(QQuickView, initialSize);
    MO_ADD_PROPERTY_RO(QQuickView, rootContext);
    MO_ADD_PROPERTY_RO(QQuickView, rootObject);

    MO_ADD_METAOBJECT1(QQuickItem, QObject);
    MO_ADD_PROPERTY(QQuickItem, acceptHoverEvents, setAcceptHoverEvents);
    MO_ADD_PROPERTY(QQuickItem, acceptedMouseButtons, setAcceptedMouseButtons);
    MO_ADD_PROPERTY(QQuickItem, cursor, setCursor);
    MO_ADD_PROPERTY(QQuickItem, filtersChildMouseEvents, setFiltersChildMouseEvents);
    MO_ADD_PROPERTY(QQuickItem, flags, setFlags);
    MO_ADD_PROPERTY_RO(QQuickItem, isFocusScope);
    MO_ADD_PROPERTY_RO(QQuickItem, isTextureProvider);
    MO_ADD_PROPERTY(QQuickItem, keepMouseGrab, setKeepMouseGrab);
    MO_ADD_PROPERTY(QQuickItem, keepTouchGrab, setKeepTouchGrab);
    // MO_ADD_PROPERTY_RO(QQuickItem, nextItemInFocusChain); // FIXME fails on the default argument
    MO_ADD_PROPERTY_RO(QQuickItem, scopedFocusItem);
    MO_ADD_PROPERTY_RO(QQuickItem, window);

    MO_ADD_METAOBJECT1(QQuickPaintedItem, QQuickItem);
    MO_ADD_PROPERTY_RO(QQuickPaintedItem, contentsBoundingRect);
    MO_ADD_PROPERTY(QQuickPaintedItem, mipmap, setMipmap);
    MO_ADD_PROPERTY(QQuickPaintedItem, opaquePainting, setOpaquePainting);
    MO_ADD_PROPERTY(QQuickPaintedItem, performanceHints, setPerformanceHints);

    MO_ADD_METAOBJECT1(QSGTexture, QObject);
    MO_ADD_PROPERTY(QSGTexture, filtering, setFiltering);
    MO_ADD_PROPERTY_RO(QSGTexture, hasAlphaChannel);
    MO_ADD_PROPERTY_RO(QSGTexture, hasMipmaps);
    MO_ADD_PROPERTY(QSGTexture, horizontalWrapMode, setHorizontalWrapMode);
    MO_ADD_PROPERTY_RO(QSGTexture, isAtlasTexture);
    MO_ADD_PROPERTY(QSGTexture, mipmapFiltering, setMipmapFiltering);
    MO_ADD_PROPERTY_RO(QSGTexture, normalizedTextureSubRect);
    MO_ADD_PROPERTY_RO(QSGTexture, textureId);
    MO_ADD_PROPERTY_RO(QSGTexture, textureSize);
    MO_ADD_PROPERTY(QSGTexture, verticalWrapMode, setVerticalWrapMode);

    MO_ADD_METAOBJECT0(QSGNode);
    MO_ADD_PROPERTY_RO(QSGNode, parent);
    MO_ADD_PROPERTY_RO(QSGNode, childCount);
    MO_ADD_PROPERTY_RO(QSGNode, flags);
    MO_ADD_PROPERTY_RO(QSGNode, isSubtreeBlocked);
    MO_ADD_PROPERTY(QSGNode, dirtyState, markDirty);

    MO_ADD_METAOBJECT1(QSGBasicGeometryNode, QSGNode);
    MO_ADD_PROPERTY_O1(QSGBasicGeometryNode, geometry);
    MO_ADD_PROPERTY_RO(QSGBasicGeometryNode, matrix);
    MO_ADD_PROPERTY_RO(QSGBasicGeometryNode, clipList);

    MO_ADD_METAOBJECT1(QSGGeometryNode, QSGBasicGeometryNode);
    MO_ADD_PROPERTY(QSGGeometryNode, material, setMaterial);
    MO_ADD_PROPERTY(QSGGeometryNode, opaqueMaterial, setOpaqueMaterial);
    MO_ADD_PROPERTY_RO(QSGGeometryNode, activeMaterial);
    MO_ADD_PROPERTY(QSGGeometryNode, renderOrder, setRenderOrder);
    MO_ADD_PROPERTY(QSGGeometryNode, inheritedOpacity, setInheritedOpacity);

    MO_ADD_METAOBJECT1(QSGClipNode, QSGBasicGeometryNode);
    MO_ADD_PROPERTY(QSGClipNode, isRectangular, setIsRectangular);
    MO_ADD_PROPERTY(QSGClipNode, clipRect, setClipRect);

    MO_ADD_METAOBJECT1(QSGTransformNode, QSGNode);
    MO_ADD_PROPERTY(QSGTransformNode, matrix, setMatrix);
    MO_ADD_PROPERTY(QSGTransformNode, combinedMatrix, setCombinedMatrix);

    MO_ADD_METAOBJECT1(QSGRootNode, QSGNode);

    MO_ADD_METAOBJECT1(QSGOpacityNode, QSGNode);
    MO_ADD_PROPERTY(QSGOpacityNode, opacity, setOpacity);
    MO_ADD_PROPERTY(QSGOpacityNode, combinedOpacity, setCombinedOpacity);

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    MO_ADD_METAOBJECT1(QSGRenderNode, QSGNode);
    MO_ADD_PROPERTY_RO(QSGRenderNode, changedStates);
    MO_ADD_PROPERTY_RO(QSGRenderNode, flags);
    MO_ADD_PROPERTY_RO(QSGRenderNode, rect);
    MO_ADD_PROPERTY_RO(QSGRenderNode, inheritedOpacity);
    MO_ADD_PROPERTY_RO(QSGRenderNode, matrix);
    MO_ADD_PROPERTY_RO(QSGRenderNode, clipList);
#endif

    MO_ADD_METAOBJECT0(QSGMaterial);
    MO_ADD_PROPERTY_RO(QSGMaterial, flags);

    MO_ADD_METAOBJECT1(QSGFlatColorMaterial, QSGMaterial);
    MO_ADD_PROPERTY(QSGFlatColorMaterial, color, setColor);

    MO_ADD_METAOBJECT1(QSGOpaqueTextureMaterial, QSGMaterial);
    MO_ADD_PROPERTY(QSGOpaqueTextureMaterial, filtering, setFiltering);
    MO_ADD_PROPERTY(QSGOpaqueTextureMaterial, horizontalWrapMode, setHorizontalWrapMode);
    MO_ADD_PROPERTY(QSGOpaqueTextureMaterial, mipmapFiltering, setMipmapFiltering);
    MO_ADD_PROPERTY(QSGOpaqueTextureMaterial, texture, setTexture);
    MO_ADD_PROPERTY(QSGOpaqueTextureMaterial, verticalWrapMode, setVerticalWrapMode);
    MO_ADD_METAOBJECT1(QSGTextureMaterial, QSGOpaqueTextureMaterial);

    MO_ADD_METAOBJECT1(QSGVertexColorMaterial, QSGMaterial);

    MO_ADD_METAOBJECT1(QSGDistanceFieldTextMaterial, QSGMaterial);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldTextMaterial, color);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldTextMaterial, fontScale);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldTextMaterial, textureSize);

    MO_ADD_METAOBJECT1(QSGDistanceFieldStyledTextMaterial, QSGDistanceFieldTextMaterial);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldStyledTextMaterial, styleColor);

    MO_ADD_METAOBJECT1(QSGDistanceFieldShiftedStyleTextMaterial, QSGDistanceFieldStyledTextMaterial);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldShiftedStyleTextMaterial, shift);

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    MO_ADD_METAOBJECT1(QQuickOpenGLShaderEffectMaterial, QSGMaterial);
    MO_ADD_PROPERTY_MEM(QQuickOpenGLShaderEffectMaterial, attributes);
    MO_ADD_PROPERTY_MEM(QQuickOpenGLShaderEffectMaterial, cullMode);
    MO_ADD_PROPERTY_MEM(QQuickOpenGLShaderEffectMaterial, geometryUsesTextureSubRect);
    MO_ADD_PROPERTY_MEM(QQuickOpenGLShaderEffectMaterial, textureProviders);
#endif
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
#define E(x) { QSGRendererInterface:: x, #x }
static const MetaEnum::Value<QSGRendererInterface::GraphicsApi> qsg_graphics_api_table[] = {
    E(Unknown),
    E(Software),
    E(OpenGL),
    E(Direct3D12),
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    E(OpenVG)
#endif
};

static const MetaEnum::Value<QSGRendererInterface::ShaderCompilationType> qsg_shader_compilation_type_table[] = {
    E(RuntimeCompilation),
    E(OfflineCompilation)
};

static const MetaEnum::Value<QSGRendererInterface::ShaderSourceType> qsg_shader_source_type_table[] = {
    E(ShaderSourceString),
    E(ShaderSourceFile),
    E(ShaderByteCode)
};

static const MetaEnum::Value<QSGRendererInterface::ShaderType> qsg_shader_type_table[] = {
    E(UnknownShadingLanguage),
    E(GLSL),
    E(HLSL)
};
#undef E
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
#define E(x) { QSGRenderNode:: x, #x }
static const MetaEnum::Value<QSGRenderNode::StateFlag> render_node_state_flags_table[] = {
    E(DepthState),
    E(StencilState),
    E(ScissorState),
    E(ColorState),
    E(BlendState),
    E(CullState),
    E(CullState),
    E(ViewportState),
    E(RenderTargetState)
};

static const MetaEnum::Value<QSGRenderNode::RenderingFlag> render_node_rendering_flags_table[] = {
    E(BoundedRectRendering),
    E(DepthAwareRendering),
    E(OpaqueRendering)
};
#undef E
#endif

void QuickInspector::registerVariantHandlers()
{
    VariantHandler::registerStringConverter<QQuickItem::Flags>(qQuickItemFlagsToString);
    VariantHandler::registerStringConverter<QQuickPaintedItem::PerformanceHints>(
        qQuickPaintedItemPerformanceHintsToString);
    VariantHandler::registerStringConverter<QSGNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGBasicGeometryNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGGeometryNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGClipNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<const QSGClipNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGTransformNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGRootNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGOpacityNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGNode::Flags>(qSGNodeFlagsToString);
    VariantHandler::registerStringConverter<QSGNode::DirtyState>(qSGNodeDirtyStateToString);
    VariantHandler::registerStringConverter<QSGGeometry *>(Util::addressToString);
    VariantHandler::registerStringConverter<const QSGGeometry *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGMaterial *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGMaterial::Flags>(qsgMaterialFlagsToString);
    VariantHandler::registerStringConverter<QSGTexture::Filtering>(qsgTextureFilteringToString);
    VariantHandler::registerStringConverter<QSGTexture::WrapMode>(qsgTextureWrapModeToString);
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    VariantHandler::registerStringConverter<QSGRenderNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGRenderNode::StateFlags>(MetaEnum::flagsToString_fn(render_node_state_flags_table));
    VariantHandler::registerStringConverter<QSGRenderNode::RenderingFlags>(MetaEnum::flagsToString_fn(render_node_rendering_flags_table));
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    VariantHandler::registerStringConverter<QSGRendererInterface*>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGRendererInterface::GraphicsApi>(MetaEnum::enumToString_fn(qsg_graphics_api_table));
    VariantHandler::registerStringConverter<QSGRendererInterface::ShaderCompilationTypes>(MetaEnum::flagsToString_fn(qsg_shader_compilation_type_table));
    VariantHandler::registerStringConverter<QSGRendererInterface::ShaderSourceTypes>(MetaEnum::flagsToString_fn(qsg_shader_source_type_table));
    VariantHandler::registerStringConverter<QSGRendererInterface::ShaderType>(MetaEnum::enumToString_fn(qsg_shader_type_table));
#endif
}

void QuickInspector::registerPCExtensions()
{
    PropertyController::registerExtension<MaterialExtension>();
    PropertyController::registerExtension<SGGeometryExtension>();
    PropertyController::registerExtension<QuickPaintAnalyzerExtension>();
    PropertyController::registerExtension<TextureExtension>();

    PropertyAdaptorFactory::registerFactory(QQuickOpenGLShaderEffectMaterialAdaptorFactory::instance());

    QmlBindingModel::registerBindingProvider(std::unique_ptr<AbstractBindingProvider>(new QuickImplicitBindingDependencyProvider));
}

#define QSG_CHECK_TYPE(Class) \
    if (dynamic_cast<Class *>(node) \
        && MetaObjectRepository::instance()->hasMetaObject(QStringLiteral(#Class))) \
        return QStringLiteral(#Class)

QString QuickInspector::findSGNodeType(QSGNode *node) const
{
    // keep this in reverse topological order of the class hierarchy!

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    QSG_CHECK_TYPE(QSGRenderNode);
#endif
    QSG_CHECK_TYPE(QSGClipNode);
    QSG_CHECK_TYPE(QSGGeometryNode);
    QSG_CHECK_TYPE(QSGBasicGeometryNode);
    QSG_CHECK_TYPE(QSGTransformNode);
    QSG_CHECK_TYPE(QSGRootNode);
    QSG_CHECK_TYPE(QSGOpacityNode);

    return QStringLiteral("QSGNode");
}
