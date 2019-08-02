/*
  qmlsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickanchorspropertyadaptor.h"
#include "quickitemmodel.h"
#include "quickscenegraphmodel.h"
#include "quickscreengrabber.h"
#include "quickpaintanalyzerextension.h"
#include "geometryextension/sggeometryextension.h"
#include "materialextension/materialextension.h"
#include "materialextension/qquickopenglshadereffectmaterialadaptor.h"
#include "textureextension/qsgtexturegrabber.h"
#include "textureextension/textureextension.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
#include "quickimplicitbindingdependencyprovider.h"
#endif

#include <common/endpoint.h>
#include <common/modelevent.h>
#include <common/objectbroker.h>
#include <common/probecontrollerinterface.h>
#include <common/problem.h>
#include <common/remoteviewframe.h>

#include <core/enumrepositoryserver.h>
#include <core/metaenum.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/probeguard.h>
#include <core/propertycontroller.h>
#include <core/propertyfilter.h>
#include <core/remote/server.h>
#include <core/remote/serverproxymodel.h>
#include <core/singlecolumnobjectproxymodel.h>
#include <core/varianthandler.h>
#include <core/remoteviewserver.h>
#include <core/paintanalyzer.h>
#include <core/bindingaggregator.h>
#include <core/problemcollector.h>

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
#include <QMutexLocker>

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
#include <QSGRenderNode>
#include <QSGRendererInterface>
#include <private/qquickopenglshadereffectnode_p.h>
#include <private/qsgsoftwarecontext_p.h>
#include <private/qsgsoftwarerenderer_p.h>
#include <private/qsgsoftwarerenderablenode_p.h>
#endif

#include <private/qquickanchors_p.h>
#include <private/qquickitem_p.h>
#include <private/qsgbatchrenderer_p.h>
#include <private/qsgdistancefieldglyphnode_p_p.h>
#include <private/qabstractanimation_p.h>

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
Q_DECLARE_METATYPE(QSGTexture::AnisotropyLevel)
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
Q_DECLARE_METATYPE(QSGRenderNode *)
Q_DECLARE_METATYPE(QSGRenderNode::RenderingFlags)
Q_DECLARE_METATYPE(QSGRenderNode::StateFlags)
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
Q_DECLARE_METATYPE(QSGRendererInterface*)
Q_DECLARE_METATYPE(QSGRendererInterface::GraphicsApi)
Q_DECLARE_METATYPE(QSGRendererInterface::ShaderCompilationTypes)
Q_DECLARE_METATYPE(QSGRendererInterface::ShaderSourceTypes)
Q_DECLARE_METATYPE(QSGRendererInterface::ShaderType)
#endif

using namespace GammaRay;

#define E(x) { QQuickItem:: x, #x }
static const MetaEnum::Value<QQuickItem::Flag> qqitem_flag_table[] = {
    E(ItemClipsChildrenToShape),
    E(ItemAcceptsInputMethod),
    E(ItemIsFocusScope),
    E(ItemHasContents),
    E(ItemAcceptsDrops)
};
#undef E

static QString qQuickPaintedItemPerformanceHintsToString(QQuickPaintedItem::PerformanceHints hints)
{
    QStringList list;
    if (hints & QQuickPaintedItem::FastFBOResizing)
        list << QStringLiteral("FastFBOResizing");
    if (list.isEmpty())
        return QStringLiteral("<none>");
    return list.join(QStringLiteral(" | "));
}

#define E(x) { QSGNode:: x, #x }
static const MetaEnum::Value<QSGNode::Flag> qsg_node_flag_table[] = {
    E(OwnedByParent),
    E(UsePreprocess),
    E(OwnsGeometry),
    E(OwnsMaterial),
    E(OwnsOpaqueMaterial)
};

static const MetaEnum::Value<QSGNode::DirtyStateBit> qsg_node_dirtystate_table[] = {
    E(DirtySubtreeBlocked),
    E(DirtyMatrix),
    E(DirtyNodeAdded),
    E(DirtyNodeRemoved),
    E(DirtyGeometry),
    E(DirtyMaterial),
    E(DirtyOpacity),
    E(DirtyForceUpdate),
    E(DirtyUsePreprocess),
    E(DirtyPropagationMask)
};
#undef E

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

#define E(x) { QSGTexture:: x, #x }
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
static const MetaEnum::Value<QSGTexture::AnisotropyLevel> qsg_texture_anisotropy_table[] = {
    E(AnisotropyNone),
    E(Anisotropy2x),
    E(Anisotropy4x),
    E(Anisotropy8x),
    E(Anisotropy16x)
};
#endif

static const MetaEnum::Value<QSGTexture::Filtering> qsg_texture_filtering_table[] = {
    E(None),
    E(Nearest),
    E(Linear)
};

static const MetaEnum::Value<QSGTexture::WrapMode> qsg_texture_wrapmode_table[] = {
    E(Repeat),
    E(ClampToEdge),
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    E(MirroredRepeat)
#endif
};

#undef E

static bool isGoodCandidateItem(QQuickItem *item)
{

    if (!item->isVisible() || qFuzzyCompare(item->opacity() + qreal(1.0), qreal(1.0)) ||
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
    if (toWindow) {
        QMutexLocker lock(&mutex);
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
    }
}

void RenderModeRequest::apply()
{
    QMutexLocker lock(&mutex);

    if (connection)
        disconnect(connection);

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    if (window && window->rendererInterface()->graphicsApi() != QSGRendererInterface::OpenGL)
        return;
#endif

    if (window) {
        emit aboutToCleanSceneGraph();
        const QByteArray mode = renderModeToString(RenderModeRequest::mode);
        QQuickWindowPrivate *winPriv = QQuickWindowPrivate::get(window);
        QMetaObject::invokeMethod(window, "cleanupSceneGraph", Qt::DirectConnection);
        winPriv->customRenderMode = mode;
        emit sceneGraphCleanedUp();
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

QuickInspector::QuickInspector(Probe *probe, QObject *parent)
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
    , m_paintAnalyzer(new PaintAnalyzer(QStringLiteral("com.kdab.GammaRay.QuickPaintAnalyzer"), this))
    , m_slowDownEnabled(false)
{
    registerMetaTypes();
    registerVariantHandlers();
    probe->installGlobalEventFilter(this);

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
        connect(m_probe, &Probe::objectCreated, this, &QuickInspector::objectCreated);
    }

    connect(probe, &Probe::objectCreated, m_itemModel, &QuickItemModel::objectAdded);
    connect(probe, &Probe::objectDestroyed, m_itemModel, &QuickItemModel::objectRemoved);
    connect(probe, &Probe::objectSelected, this, &QuickInspector::qObjectSelected);
    connect(probe, &Probe::nonQObjectSelected, this, &QuickInspector::nonQObjectSelected);

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
    connect(probe, &Probe::objectCreated, texGrab, &QSGTextureGrabber::objectCreated);

    connect(Endpoint::instance(), &Endpoint::disconnected, this, [this]() {
        if (m_overlay)
            m_overlay->placeOn(ItemOrLayoutFacade());
    });

    ProblemCollector::registerProblemChecker("com.kdab.GammaRay.QuickItemChecker",
                                          "QtQuick Item check",
                                          "Warns about items that are visible but out of view.",
                                          &QuickInspector::scanForProblems);

    // needs to be last, extensions require some of the above to be set up correctly
    registerPCExtensions();
}

QuickInspector::~QuickInspector()
{
    if (m_overlay) {
        disconnect(m_overlay.get(), &QObject::destroyed, this, &QuickInspector::recreateOverlay);
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
    recreateOverlay();

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

void QuickInspector::qObjectSelected(QObject *object)
{
    if (auto item = qobject_cast<QQuickItem *>(object))
        selectItem(item);
    else if (auto window = qobject_cast<QQuickWindow *>(object))
        selectWindow(window);
}

void QuickInspector::nonQObjectSelected(void *object, const QString &typeName)
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
    if (m_overlay)
        disconnect(m_overlay.get(), &QObject::destroyed, this, &QuickInspector::recreateOverlay);

    m_overlay = AbstractScreenGrabber::get(m_window);

    connect(m_overlay.get(), &AbstractScreenGrabber::grabberReadyChanged, m_remoteView, &RemoteViewServer::setGrabberReady);
    connect(m_overlay.get(), &AbstractScreenGrabber::sceneChanged, m_remoteView, &RemoteViewServer::sourceChanged);
    connect(m_overlay.get(), &AbstractScreenGrabber::sceneGrabbed, this, &QuickInspector::sendRenderedScene);
    // the target application might have destroyed the overlay widget
    // (e.g. because the parent of the overlay got destroyed).
    // just recreate a new one in this case
    connect(m_overlay.get(), &QObject::destroyed, this, &QuickInspector::recreateOverlay); //FIXME Is it really needed?
                                                                                           // It is for the widget inspector, but for qt quick?
    connect(this, &QuickInspectorInterface::serverSideDecorationChanged, m_overlay.get(), &AbstractScreenGrabber::setDecorationsEnabled);
    m_overlay->setDecorationsEnabled(serverSideDecorationEnabled());

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
    if (m_overlay && m_overlay->settings().componentsTraces)
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
    if (m_overlay) {
        m_overlay->requestGrabWindow(m_remoteView->userViewport());
    }
}

void QuickInspector::setCustomRenderMode(
    GammaRay::QuickInspectorInterface::RenderMode customRenderMode)
{
    m_renderMode = customRenderMode;

    m_pendingRenderMode->applyOrDelay(m_window, customRenderMode);

    const bool tracing = customRenderMode == QuickInspectorInterface::VisualizeTraces;
    if (m_overlay && m_overlay->settings().componentsTraces != tracing) {
        auto settings = m_overlay->settings();
        settings.componentsTraces = tracing;
        setOverlaySettings(settings);
    }
}

void QuickInspector::checkFeatures()
{
    Features f;
    if (!m_window) {
        emit features(f);
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    if (m_window->rendererInterface()->graphicsApi() == QSGRendererInterface::OpenGL)
        f = AllCustomRenderModes;
    else if (m_window->rendererInterface()->graphicsApi() == QSGRendererInterface::Software)
        f = AnalyzePainting;
#else
    f = AllCustomRenderModes;
#endif

    emit features(f);
}

void QuickInspector::setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings)
{
    if (!m_overlay) {
        emit overlaySettings(QuickDecorationsSettings()); // Let's not leave the client without an answer.
        return;
    }

    m_overlay->setSettings(settings);
    emit overlaySettings(m_overlay->settings());
}

void QuickInspector::checkOverlaySettings()
{
    emit overlaySettings(m_overlay ? m_overlay->settings() : QuickDecorationsSettings());
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 3) // only with 5.9.3 the SW renderer got exported
class SGSoftwareRendererPrivacyViolater : public QSGAbstractSoftwareRenderer
{
public:
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    using QSGAbstractSoftwareRenderer::renderableNodes;
#endif
    using QSGAbstractSoftwareRenderer::renderNodes;
    using QSGAbstractSoftwareRenderer::buildRenderList;
    using QSGAbstractSoftwareRenderer::optimizeRenderList;
};
#endif

void QuickInspector::analyzePainting()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 3)
    if (!m_window || m_window->rendererInterface()->graphicsApi() != QSGRendererInterface::Software || !PaintAnalyzer::isAvailable())
        return;

    m_paintAnalyzer->beginAnalyzePainting();
    m_paintAnalyzer->setBoundingRect(QRect(QPoint(), m_window->size()));
    {
        auto w = QQuickWindowPrivate::get(m_window);
        auto renderer = static_cast<SGSoftwareRendererPrivacyViolater*>(w->renderer);

        // this replicates what QSGSoftwareRender is doing
        QPainter painter(m_paintAnalyzer->paintDevice());
        painter.setRenderHint(QPainter::Antialiasing);
        auto rc = static_cast<QSGSoftwareRenderContext*>(w->renderer->context());
        auto prevPainter = rc->m_activePainter;
        rc->m_activePainter = &painter;
        renderer->markDirty();
        renderer->buildRenderList();
        renderer->optimizeRenderList();
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0) || QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        renderer->renderNodes(&painter);
#else
        auto iterator = renderer->renderableNodes().begin();
        // First node is the background and needs to painted without blending
        auto backgroundNode = *iterator;
        backgroundNode->renderNode(&painter, /*force opaque painting*/ true);
        iterator++;

        for (; iterator != renderer->renderableNodes().end(); ++iterator) {
            auto node = *iterator;
            QQuickItem *origin = m_sgModel->itemForSgNode(node->handle());
            m_paintAnalyzer->setOrigin(ObjectId(origin));
            node->renderNode(&painter);
        }
#endif

        rc->m_activePainter = prevPainter;
    }
    m_paintAnalyzer->endAnalyzePainting();
#endif
}

void QuickInspector::checkSlowMode()
{
    // We can't check that for now as there is no getter for the property...
    emit slowModeChanged(m_slowDownEnabled);
}

void QuickInspector::setSlowMode(bool slow)
{
    if (m_slowDownEnabled == slow)
        return;

    static QHash<QQuickWindow *, QMetaObject::Connection> connections;

    m_slowDownEnabled = slow;

    for (int i = 0; i < m_windowModel->rowCount(); ++i) {
        const QModelIndex index = m_windowModel->index(i, 0);
        QQuickWindow *window = index.data(ObjectModel::ObjectRole).value<QQuickWindow *>();
        auto it = connections.find(window);

        if (it == connections.end()) {
            connections.insert(window, connect(window, &QQuickWindow::beforeRendering, this, [this, window]() {
                auto it = connections.find(window);
                QUnifiedTimer::instance()->setSlowModeEnabled(m_slowDownEnabled);
                QObject::disconnect(it.value());
                connections.erase(it);
            }, Qt::DirectConnection));
        }
    }

    emit slowModeChanged(m_slowDownEnabled);
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

    if (m_overlay)
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

    void *obj = m_currentSgNode;
    auto mo = MetaObjectRepository::instance()->metaObject(QStringLiteral("QSGNode"), obj);
    m_sgPropertyController->setObject(m_currentSgNode, mo->className());

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
        const auto child = childItems.at(i);
        const auto requestedPoint = parent->mapToItem(child, pos);
        if (!child->childItems().isEmpty() && (child->contains(requestedPoint) || child->childrenRect().contains(requestedPoint))) {
            const int count = objects.count();
            int bc; // possibly better candidate among subChildren
            objects << recursiveItemsAt(child, requestedPoint, mode, bc);

            if (bestCandidate == -1 && bc != -1) {
                bestCandidate = count + bc;
            }
        }

        if (child->contains(requestedPoint)) {
            if (bestCandidate == -1 && isGoodCandidateItem(child)) {
                bestCandidate = objects.count();
            }
            objects << ObjectId(child);
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


void QuickInspector::scanForProblems()
{
    const QVector<QObject*> &allObjects = Probe::instance()->allQObjects();

    QMutexLocker lock(Probe::objectLock());
    for (QObject *obj : allObjects) {
        QQuickItem *item;
        if (!Probe::instance()->isValidObject(obj) || !(item = qobject_cast<QQuickItem*>(obj)))
            continue;

        QQuickItem *ancestor = item->parentItem();
        auto rect = item->mapRectToScene(QRectF(0, 0, item->width(), item->height()));

        while (ancestor && item->window() && ancestor != item->window()->contentItem()) {
            if (ancestor->parentItem() == item->window()->contentItem() || ancestor->clip()) {
                auto ancestorRect = ancestor->mapRectToScene(QRectF(0, 0, ancestor->width(), ancestor->height()));

                if (!ancestorRect.contains(rect) && !rect.intersects(ancestorRect)) {
                    Problem p;
                    p.severity = Problem::Info;
                    p.description = QStringLiteral("QtQuick: %1 %2 (0x%3) is visible, but out of view.").arg(
                        ObjectDataProvider::typeName(item),
                        ObjectDataProvider::name(item),
                        QString::number(reinterpret_cast<quintptr>(item), 16)
                    );
                    p.object = ObjectId(item);
                    p.locations.push_back(ObjectDataProvider::creationLocation(item));
                    p.problemId = QStringLiteral("com.kdab.GammaRay.QuickItemChecker.OutOfView:%1").arg(reinterpret_cast<quintptr>(item));
                    p.findingCategory = Problem::Scan;
                    ProblemCollector::addProblem(p);
                    break;
                }
            }
            ancestor = ancestor->parentItem();
        }
    }
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
    MO_ADD_PROPERTY_RO(QQuickWindow, effectiveDevicePixelRatio);
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
    MO_ADD_PROPERTY_LD(QQuickItem, nextItemInFocusChain, [](QQuickItem *item) { return item->isVisible() ? item->nextItemInFocusChain() : nullptr; });
    MO_ADD_PROPERTY_LD(QQuickItem, previousItemInFocusChain, [](QQuickItem *item) { return item->isVisible() ? item->nextItemInFocusChain(false) : nullptr; });
    MO_ADD_PROPERTY_RO(QQuickItem, scopedFocusItem);
    MO_ADD_PROPERTY_RO(QQuickItem, window);

    MO_ADD_METAOBJECT1(QQuickPaintedItem, QQuickItem);
    MO_ADD_PROPERTY_RO(QQuickPaintedItem, contentsBoundingRect);
    MO_ADD_PROPERTY(QQuickPaintedItem, mipmap, setMipmap);
    MO_ADD_PROPERTY(QQuickPaintedItem, opaquePainting, setOpaquePainting);
    MO_ADD_PROPERTY(QQuickPaintedItem, performanceHints, setPerformanceHints);

    MO_ADD_METAOBJECT1(QSGTexture, QObject);
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    MO_ADD_PROPERTY(QSGTexture, anisotropyLevel, setAnisotropyLevel);
#endif
    MO_ADD_PROPERTY(QSGTexture, filtering, setFiltering);
    MO_ADD_PROPERTY_RO(QSGTexture, hasAlphaChannel);
    MO_ADD_PROPERTY_RO(QSGTexture, hasMipmaps);
    MO_ADD_PROPERTY(QSGTexture, horizontalWrapMode, setHorizontalWrapMode);
    MO_ADD_PROPERTY_RO(QSGTexture, isAtlasTexture);
    MO_ADD_PROPERTY(QSGTexture, mipmapFiltering, setMipmapFiltering);
    MO_ADD_PROPERTY_RO(QSGTexture, normalizedTextureSubRect);
    // crashes without a current GL context
    //MO_ADD_PROPERTY_RO(QSGTexture, textureId);
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

static QString anchorLineToString(const QQuickAnchorLine &line)
{
    if (!line.item
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        || line.anchorLine == QQuickAnchors::InvalidAnchor
#endif
    ) {
        return QStringLiteral("<none>");
    }
    const auto s = Util::shortDisplayString(line.item);
    switch (line.anchorLine) {
        case QQuickAnchors::LeftAnchor: return s + QStringLiteral(".left");
        case QQuickAnchors::RightAnchor: return s + QStringLiteral(".right");
        case QQuickAnchors::TopAnchor: return s + QStringLiteral(".top");
        case QQuickAnchors::BottomAnchor: return s + QStringLiteral(".bottom");
        case QQuickAnchors::HCenterAnchor: return s + QStringLiteral(".horizontalCenter");
        case QQuickAnchors::VCenterAnchor: return s + QStringLiteral(".verticalCenter");
        case QQuickAnchors::BaselineAnchor: return s + QStringLiteral(".baseline");
        default: break;
    }
    return s;
}

void QuickInspector::registerVariantHandlers()
{
    ER_REGISTER_FLAGS(QQuickItem, Flags, qqitem_flag_table);
    ER_REGISTER_FLAGS(QSGNode, DirtyState, qsg_node_dirtystate_table);
    ER_REGISTER_FLAGS(QSGNode, Flags, qsg_node_flag_table);
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    ER_REGISTER_ENUM(QSGTexture, AnisotropyLevel, qsg_texture_anisotropy_table);
#endif
    ER_REGISTER_ENUM(QSGTexture, Filtering, qsg_texture_filtering_table);
    ER_REGISTER_ENUM(QSGTexture, WrapMode, qsg_texture_wrapmode_table);

    VariantHandler::registerStringConverter<QQuickPaintedItem::PerformanceHints>(
        qQuickPaintedItemPerformanceHintsToString);
    VariantHandler::registerStringConverter<QQuickAnchorLine>(anchorLineToString);
    VariantHandler::registerStringConverter<QSGNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGBasicGeometryNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGGeometryNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGClipNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<const QSGClipNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGTransformNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGRootNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGOpacityNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGGeometry *>(Util::addressToString);
    VariantHandler::registerStringConverter<const QSGGeometry *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGMaterial *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGMaterial::Flags>(qsgMaterialFlagsToString);
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
    PropertyAdaptorFactory::registerFactory(QuickAnchorsPropertyAdaptorFactory::instance());
    PropertyFilters::registerFilter(PropertyFilter("QQuickItem", "anchors"));

#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    BindingAggregator::registerBindingProvider(std::unique_ptr<AbstractBindingProvider>(new QuickImplicitBindingDependencyProvider));
#endif
}
