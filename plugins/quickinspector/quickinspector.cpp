/*
  quickinspector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

#include "quickimplicitbindingdependencyprovider.h"

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

#include <QQuickItem>
#include <QQuickWindow>
#include <QQuickView>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlError>

#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMouseEvent>
#ifndef QT_NO_OPENGL
#include <QOpenGLContext>
#endif
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
#include <QSortFilterProxyModel>

#include <QSGRenderNode>
#include <QSGRendererInterface>
#include <private/qsgsoftwarecontext_p.h>
#include <private/qsgsoftwarerenderer_p.h>
#include <private/qsgsoftwarerenderablenode_p.h>

#include <private/qquickanchors_p.h>
#include <private/qquickitem_p.h>
#ifndef QT_NO_OPENGL
#include <private/qsgbatchrenderer_p.h>
#endif
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
Q_DECLARE_METATYPE(QSGTexture::AnisotropyLevel)
Q_DECLARE_METATYPE(QSGRenderNode *)
Q_DECLARE_METATYPE(QSGRenderNode::RenderingFlags)
Q_DECLARE_METATYPE(QSGRenderNode::StateFlags)
Q_DECLARE_METATYPE(QSGRendererInterface *)
Q_DECLARE_METATYPE(QSGRendererInterface::GraphicsApi)
Q_DECLARE_METATYPE(QSGRendererInterface::ShaderCompilationTypes)
Q_DECLARE_METATYPE(QSGRendererInterface::ShaderSourceTypes)
Q_DECLARE_METATYPE(QSGRendererInterface::ShaderType)

using namespace GammaRay;

#define E(x)              \
    {                     \
        QQuickItem::x, #x \
    }
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

#define E(x)           \
    {                  \
        QSGNode::x, #x \
    }
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
#define F(f)                    \
    if (flags & QSGMaterial::f) \
        list.push_back(QStringLiteral(#f));
    F(Blending)
    F(RequiresDeterminant)
    F(RequiresFullMatrixExceptTranslate)
    F(RequiresFullMatrix)
    F(NoBatching)
#undef F

    if (list.isEmpty())
        return QStringLiteral("<none>");
    return list.join(QStringLiteral(" | "));
}

#define E(x)              \
    {                     \
        QSGTexture::x, #x \
    }
static const MetaEnum::Value<QSGTexture::AnisotropyLevel> qsg_texture_anisotropy_table[] = {
    E(AnisotropyNone),
    E(Anisotropy2x),
    E(Anisotropy4x),
    E(Anisotropy8x),
    E(Anisotropy16x)
};

static const MetaEnum::Value<QSGTexture::Filtering> qsg_texture_filtering_table[] = {
    E(None),
    E(Nearest),
    E(Linear)
};

static const MetaEnum::Value<QSGTexture::WrapMode> qsg_texture_wrapmode_table[] = {
    E(Repeat),
    E(ClampToEdge),
    E(MirroredRepeat)
};

#undef E

static bool itemHasContents(QQuickItem *item)
{
    return item->flags().testFlag(QQuickItem::ItemHasContents);
}

static bool isGoodCandidateItem(QQuickItem *item, bool ignoreItemHasContents = false)
{
    return !(!item->isVisible() || qFuzzyCompare(item->opacity() + qreal(1.0), qreal(1.0)) || (!ignoreItemHasContents && !itemHasContents(item)));
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

        if (!connection || (mode != customRenderMode || window != toWindow)) {
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

    if (window && window->rendererInterface()->graphicsApi() != QSGRendererInterface::OpenGL)
        return;

    if (window) {
        const QByteArray mode = renderModeToString(RenderModeRequest::mode);
        QQuickWindowPrivate *winPriv = QQuickWindowPrivate::get(window);
        QObject::connect(window.get(), &QQuickWindow::beforeSynchronizing, this, [this, winPriv, mode]() {
            emit aboutToCleanSceneGraph();
            QMetaObject::invokeMethod(window, "cleanupSceneGraph", Qt::DirectConnection);
            winPriv->visualizationMode = mode;
            emit sceneGraphCleanedUp(); }, static_cast<Qt::ConnectionType>(Qt::DirectConnection | Qt::SingleShotConnection));
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
                                                        "com.kdab.GammaRay.QuickSceneGraph"),
                                                    this))
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

    auto filterProxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    filterProxy->setSourceModel(m_itemModel);
    filterProxy->addRole(ObjectModel::ObjectIdRole);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.QuickItemModel"), filterProxy);

    if (m_probe->needsObjectDiscovery()) {
        connect(m_probe, &Probe::objectCreated, this, &QuickInspector::objectCreated);
    }

    connect(probe, &Probe::objectCreated, m_itemModel, &QuickItemModel::objectAdded);
    connect(probe, &Probe::objectDestroyed, m_itemModel, &QuickItemModel::objectRemoved);
    connect(probe, &Probe::objectFavorited, m_itemModel, &QuickItemModel::objectFavorited);
    connect(probe, &Probe::objectUnfavorited, m_itemModel, &QuickItemModel::objectUnfavorited);
    connect(probe, &Probe::objectSelected, this, &QuickInspector::qObjectSelected);
    connect(probe, &Probe::nonQObjectSelected, this, &QuickInspector::nonQObjectSelected);

    m_itemSelectionModel = ObjectBroker::selectionModel(filterProxy);
    connect(m_itemSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &QuickInspector::itemSelectionChanged);

    auto sgFilterProxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    sgFilterProxy->setSourceModel(m_sgModel);
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"), sgFilterProxy);

    m_sgSelectionModel = ObjectBroker::selectionModel(sgFilterProxy);
    connect(m_sgSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &QuickInspector::sgSelectionChanged);
    connect(m_sgModel, &QuickSceneGraphModel::nodeDeleted, this, &QuickInspector::sgNodeDeleted);

    connect(m_remoteView, &RemoteViewServer::elementsAtRequested, this, &QuickInspector::requestElementsAt);
    connect(this, &QuickInspector::elementsAtReceived, m_remoteView, &RemoteViewServer::elementsAtReceived);
    connect(m_remoteView, &RemoteViewServer::doPickElementId, this, &QuickInspector::pickElementId);
    connect(m_remoteView, &RemoteViewServer::requestUpdate, this, &QuickInspector::slotGrabWindow);
    connect(m_pendingRenderMode, &RenderModeRequest::aboutToCleanSceneGraph, this, &QuickInspector::aboutToCleanSceneGraph);
    connect(m_pendingRenderMode, &RenderModeRequest::sceneGraphCleanedUp, this, &QuickInspector::sceneGraphCleanedUp);

#ifndef QT_NO_OPENGL
    auto texGrab = new QSGTextureGrabber(this);
    connect(probe, &Probe::objectCreated, texGrab, &QSGTextureGrabber::objectCreated);
#endif

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
        const QByteArray mode = QQuickWindowPrivate::get(m_window)->visualizationMode;

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

    const QModelIndexList indexList = model->match(model->index(0, 0),
                                                   ObjectModel::ObjectRole,
                                                   QVariant::fromValue<QQuickItem *>(item), 1,
                                                   Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const QModelIndex index = indexList.first();
    m_itemSelectionModel->select(index,
                                 QItemSelectionModel::Select
                                     | QItemSelectionModel::Clear
                                     | QItemSelectionModel::Rows
                                     | QItemSelectionModel::Current);
}

void QuickInspector::selectSGNode(QSGNode *node)
{
    const QAbstractItemModel *model = m_sgSelectionModel->model();
    Model::used(model);

    const QModelIndexList indexList = model->match(model->index(0, 0), ObjectModel::ObjectRole,
                                                   QVariant::fromValue(
                                                       node),
                                                   1,
                                                   Qt::MatchExactly | Qt::MatchRecursive
                                                       | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const QModelIndex index = indexList.first();
    m_sgSelectionModel->select(index,
                               QItemSelectionModel::Select
                                   | QItemSelectionModel::Clear
                                   | QItemSelectionModel::Rows
                                   | QItemSelectionModel::Current);
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
        } else {
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
    if (!m_overlay)
        return;

    connect(m_overlay.get(), &AbstractScreenGrabber::grabberReadyChanged, m_remoteView, &RemoteViewServer::setGrabberReady);
    connect(m_overlay.get(), &AbstractScreenGrabber::sceneChanged, m_remoteView, &RemoteViewServer::sourceChanged);
    connect(m_overlay.get(), &AbstractScreenGrabber::sceneGrabbed, this, &QuickInspector::sendRenderedScene);
    // the target application might have destroyed the overlay widget
    // (e.g. because the parent of the overlay got destroyed).
    // just recreate a new one in this case
    connect(m_overlay.get(), &QObject::destroyed, this, &QuickInspector::recreateOverlay); // FIXME Is it really needed?
                                                                                           //  It is for the widget inspector, but for qt quick?
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
    if (!m_window)
        return;
    RemoteViewFrame frame;
    frame.setImage(grabbedFrame.image, grabbedFrame.transform);
    frame.setSceneRect(grabbedFrame.itemsGeometryRect);
    frame.setViewRect(QRect(0, 0, m_window->width(), m_window->height()));
    if (m_overlay && m_overlay->settings().componentsTraces)
        frame.data = QVariant::fromValue(grabbedFrame.itemsGeometry);
    else if (!grabbedFrame.itemsGeometry.isEmpty())
        frame.data = QVariant::fromValue(grabbedFrame.itemsGeometry.at(0));
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

    if (m_window->rendererInterface()->graphicsApi() == QSGRendererInterface::OpenGL)
        f = AllCustomRenderModes;
    else if (m_window->rendererInterface()->graphicsApi() == QSGRendererInterface::Software)
        f = AnalyzePainting;

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

class SGSoftwareRendererPrivacyViolater : public QSGAbstractSoftwareRenderer
{
public:
    using QSGAbstractSoftwareRenderer::buildRenderList;
    using QSGAbstractSoftwareRenderer::optimizeRenderList;
    using QSGAbstractSoftwareRenderer::renderNodes;
};

#if defined(Q_CC_CLANG) || defined(Q_CC_GNU)
// keep it working in UBSAN
__attribute__((no_sanitize("vptr")))
#endif
void
QuickInspector::analyzePainting()
{
    if (!m_window || m_window->rendererInterface()->graphicsApi() != QSGRendererInterface::Software || !PaintAnalyzer::isAvailable())
        return;

    m_paintAnalyzer->beginAnalyzePainting();
    m_paintAnalyzer->setBoundingRect(QRect(QPoint(), m_window->size()));
    {
        auto w = QQuickWindowPrivate::get(m_window);
        auto renderer = static_cast<SGSoftwareRendererPrivacyViolater *>(w->renderer);

        // this replicates what QSGSoftwareRender is doing
        QPainter painter(m_paintAnalyzer->paintDevice());
        painter.setRenderHint(QPainter::Antialiasing);
        auto rc = static_cast<QSGSoftwareRenderContext *>(w->renderer->context());
        auto prevPainter = rc->m_activePainter;
        rc->m_activePainter = &painter;
        renderer->markDirty();
        renderer->buildRenderList();
        renderer->optimizeRenderList();
        renderer->renderNodes(&painter);

        rc->m_activePainter = prevPainter;
    }
    m_paintAnalyzer->endAnalyzePainting();
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
                                               connections.erase(it); }, Qt::DirectConnection));
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
                                       | QItemSelectionModel::Clear
                                       | QItemSelectionModel::Rows
                                       | QItemSelectionModel::Current);
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

QRectF QuickInspector::combinedChildrenRect(QQuickItem *parent) const
{
    auto rect = parent->childrenRect();

    const auto childItems = parent->childItems();
    for (const auto child : childItems) {
        auto childRect = child->childrenRect();

        // Get Global positon of childRect
        QPointF childGlobalPos = child->mapToScene(QPointF(0, 0));

        // Convert global position to local coordinates of the parent object
        QPointF localChildPos = parent->mapFromScene(childGlobalPos);

        // Adjust childRect to be in local coordinates of the parent object
        childRect.moveTopLeft(localChildPos.toPoint());

        // Adding the childRect to the rect
        rect = rect.united(childRect);
    }

    return rect;
}

ObjectIds QuickInspector::recursiveItemsAt(QQuickItem *parent, const QPointF &pos,
                                           GammaRay::RemoteViewInterface::RequestMode mode,
                                           int &bestCandidate, bool parentIsGoodCandidate) const
{
    Q_ASSERT(parent);
    ObjectIds objects;

    bestCandidate = -1;
    if (parentIsGoodCandidate) {
        // inherit the parent item opacity when looking for a good candidate item
        // i.e. QQuickItem::isVisible is taking the parent into account already, but
        // the opacity doesn't - we have to do this manually
        // Yet we have to ignore ItemHasContents apparently, as the QQuickRootItem
        // at least seems to not have this flag set.
        parentIsGoodCandidate = isGoodCandidateItem(parent, true);
    }

    auto childItems = parent->childItems();
    std::stable_sort(childItems.begin(), childItems.end(),
                     [](QQuickItem *lhs, QQuickItem *rhs) { return lhs->z() < rhs->z(); });

    for (int i = childItems.size() - 1; i >= 0; --i) { // backwards to match z order
        const auto child = childItems.at(i);
        const auto requestedPoint = parent->mapToItem(child, pos);
        if (!child->childItems().isEmpty() && (child->contains(requestedPoint) || combinedChildrenRect(child).contains(requestedPoint))) {
            const int count = objects.size();
            int bc; // possibly better candidate among subChildren
            objects << recursiveItemsAt(child, requestedPoint, mode, bc, parentIsGoodCandidate);

            if (bestCandidate == -1 && parentIsGoodCandidate && bc != -1) {
                bestCandidate = count + bc;
            }
        }

        if (child->contains(requestedPoint)) {
            if (bestCandidate == -1 && parentIsGoodCandidate && isGoodCandidateItem(child)) {
                bestCandidate = objects.size();
            }
            objects << ObjectId(child);
        }

        if (bestCandidate != -1 && mode == RemoteViewInterface::RequestBest) {
            break;
        }
    }

    if (bestCandidate == -1 && parentIsGoodCandidate && itemHasContents(parent)) {
        bestCandidate = objects.size();
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
    const QVector<QObject *> &allObjects = Probe::instance()->allQObjects();

    QMutexLocker lock(Probe::objectLock());
    for (QObject *obj : allObjects) {
        QQuickItem *item;
        if (!Probe::instance()->isValidObject(obj) || !(item = qobject_cast<QQuickItem *>(obj)))
            continue;

        QQuickItem *ancestor = item->parentItem();
        auto rect = item->mapRectToScene(QRectF(0, 0, item->width(), item->height()));

        while (ancestor && item->window() && ancestor != item->window()->contentItem()) {
            if (ancestor->parentItem() == item->window()->contentItem() || ancestor->clip()) {
                auto ancestorRect = ancestor->mapRectToScene(QRectF(0, 0, ancestor->width(), ancestor->height()));

                if (!ancestorRect.contains(rect) && !rect.intersects(ancestorRect)) {
                    Problem p;
                    p.severity = Problem::Info;
                    p.description = QStringLiteral("QtQuick: %1 %2 (0x%3) is visible, but out of view.").arg(ObjectDataProvider::typeName(item), ObjectDataProvider::name(item), QString::number(reinterpret_cast<quintptr>(item), 16));
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
        QMouseEvent *mouseEv = static_cast<QMouseEvent *>(event);
        if (mouseEv->button() == Qt::LeftButton && mouseEv->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
            QQuickWindow *window = qobject_cast<QQuickWindow *>(receiver);
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

    MO_ADD_PROPERTY(QQuickWindow, renderTarget, setRenderTarget);
    MO_ADD_PROPERTY(QQuickWindow, graphicsConfiguration, setGraphicsConfiguration);
    MO_ADD_PROPERTY(QQuickWindow, graphicsDevice, setGraphicsDevice);

#ifndef QT_NO_OPENGL
    MO_ADD_PROPERTY(QQuickWindow, isPersistentGraphics, setPersistentGraphics);
#endif

    MO_ADD_PROPERTY_RO(QQuickWindow, mouseGrabberItem);
    MO_ADD_PROPERTY(QQuickWindow, isPersistentSceneGraph, setPersistentSceneGraph);
    MO_ADD_PROPERTY_RO(QQuickWindow, effectiveDevicePixelRatio);

    MO_ADD_PROPERTY_RO(QQuickWindow, rendererInterface);

    MO_ADD_METAOBJECT0(QSGRendererInterface);
    MO_ADD_PROPERTY_RO(QSGRendererInterface, graphicsApi);
    MO_ADD_PROPERTY_RO(QSGRendererInterface, shaderCompilationType);
    MO_ADD_PROPERTY_RO(QSGRendererInterface, shaderSourceType);
    MO_ADD_PROPERTY_RO(QSGRendererInterface, shaderType);

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
    MO_ADD_PROPERTY(QSGTexture, anisotropyLevel, setAnisotropyLevel);
    MO_ADD_PROPERTY(QSGTexture, filtering, setFiltering);
    MO_ADD_PROPERTY_RO(QSGTexture, hasAlphaChannel);
    MO_ADD_PROPERTY_RO(QSGTexture, hasMipmaps);
    MO_ADD_PROPERTY(QSGTexture, horizontalWrapMode, setHorizontalWrapMode);
    MO_ADD_PROPERTY_RO(QSGTexture, isAtlasTexture);
    MO_ADD_PROPERTY(QSGTexture, mipmapFiltering, setMipmapFiltering);
    MO_ADD_PROPERTY_RO(QSGTexture, normalizedTextureSubRect);
    // crashes without a current GL context
    // MO_ADD_PROPERTY_RO(QSGTexture, textureId);
    MO_ADD_PROPERTY_RO(QSGTexture, textureSize);
    MO_ADD_PROPERTY(QSGTexture, verticalWrapMode, setVerticalWrapMode);

    MO_ADD_METAOBJECT0(QSGNode);
    MO_ADD_PROPERTY_RO(QSGNode, parent);
    MO_ADD_PROPERTY_RO(QSGNode, childCount);
    MO_ADD_PROPERTY_RO(QSGNode, flags);
    MO_ADD_PROPERTY_RO(QSGNode, isSubtreeBlocked);
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    MO_ADD_PROPERTY(QSGNode, dirtyState, markDirty); // NOLINT
#endif

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

    MO_ADD_METAOBJECT1(QSGRenderNode, QSGNode);
    MO_ADD_PROPERTY_RO(QSGRenderNode, changedStates);
    MO_ADD_PROPERTY_RO(QSGRenderNode, flags);
    MO_ADD_PROPERTY_RO(QSGRenderNode, rect);
    MO_ADD_PROPERTY_RO(QSGRenderNode, inheritedOpacity);
    MO_ADD_PROPERTY_RO(QSGRenderNode, matrix);
    MO_ADD_PROPERTY_RO(QSGRenderNode, clipList);

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

#ifndef QT_NO_OPENGL
    MO_ADD_METAOBJECT1(QSGDistanceFieldTextMaterial, QSGMaterial);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldTextMaterial, color);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldTextMaterial, fontScale);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldTextMaterial, textureSize);

    MO_ADD_METAOBJECT1(QSGDistanceFieldStyledTextMaterial, QSGDistanceFieldTextMaterial);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldStyledTextMaterial, styleColor);

    MO_ADD_METAOBJECT1(QSGDistanceFieldShiftedStyleTextMaterial, QSGDistanceFieldStyledTextMaterial);
    MO_ADD_PROPERTY_RO(QSGDistanceFieldShiftedStyleTextMaterial, shift);
#endif
}

#define E(x)                        \
    {                               \
        QSGRendererInterface::x, #x \
    }
static const MetaEnum::Value<QSGRendererInterface::GraphicsApi> qsg_graphics_api_table[] = {
    E(Unknown),
    E(Software),
    E(OpenGL),
    E(OpenVG),
    E(Direct3D11),
    E(Vulkan),
    E(Metal),
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

#define E(x)                 \
    {                        \
        QSGRenderNode::x, #x \
    }
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

static QString anchorLineToString(const QQuickAnchorLine &line)
{
    if (!line.item
        || line.anchorLine == QQuickAnchors::InvalidAnchor) {
        return QStringLiteral("<none>");
    }
    QString s = Util::shortDisplayString(line.item);
    switch (line.anchorLine) {
    case QQuickAnchors::LeftAnchor:
        return s + QStringLiteral(".left");
    case QQuickAnchors::RightAnchor:
        return s + QStringLiteral(".right");
    case QQuickAnchors::TopAnchor:
        return s + QStringLiteral(".top");
    case QQuickAnchors::BottomAnchor:
        return s + QStringLiteral(".bottom");
    case QQuickAnchors::HCenterAnchor:
        return s + QStringLiteral(".horizontalCenter");
    case QQuickAnchors::VCenterAnchor:
        return s + QStringLiteral(".verticalCenter");
    case QQuickAnchors::BaselineAnchor:
        return s + QStringLiteral(".baseline");
    default:
        break;
    }
    return s;
}

void QuickInspector::registerVariantHandlers()
{
    ER_REGISTER_FLAGS(QQuickItem, Flags, qqitem_flag_table);
    ER_REGISTER_FLAGS(QSGNode, DirtyState, qsg_node_dirtystate_table);
    ER_REGISTER_FLAGS(QSGNode, Flags, qsg_node_flag_table);
    ER_REGISTER_ENUM(QSGTexture, AnisotropyLevel, qsg_texture_anisotropy_table);
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
    VariantHandler::registerStringConverter<QSGRenderNode *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGRenderNode::StateFlags>(MetaEnum::flagsToString_fn(render_node_state_flags_table));
    VariantHandler::registerStringConverter<QSGRenderNode::RenderingFlags>(MetaEnum::flagsToString_fn(render_node_rendering_flags_table));

    VariantHandler::registerStringConverter<QSGRendererInterface *>(Util::addressToString);
    VariantHandler::registerStringConverter<QSGRendererInterface::GraphicsApi>(MetaEnum::enumToString_fn(qsg_graphics_api_table));
    VariantHandler::registerStringConverter<QSGRendererInterface::ShaderCompilationTypes>(MetaEnum::flagsToString_fn(qsg_shader_compilation_type_table));
    VariantHandler::registerStringConverter<QSGRendererInterface::ShaderSourceTypes>(MetaEnum::flagsToString_fn(qsg_shader_source_type_table));
    VariantHandler::registerStringConverter<QSGRendererInterface::ShaderType>(MetaEnum::enumToString_fn(qsg_shader_type_table));
}

void QuickInspector::registerPCExtensions()
{
#ifndef QT_NO_OPENGL
    PropertyController::registerExtension<MaterialExtension>();
    PropertyController::registerExtension<SGGeometryExtension>();
    PropertyController::registerExtension<QuickPaintAnalyzerExtension>();
    PropertyController::registerExtension<TextureExtension>();

    PropertyAdaptorFactory::registerFactory(QQuickOpenGLShaderEffectMaterialAdaptorFactory::instance());
#endif
    PropertyAdaptorFactory::registerFactory(QuickAnchorsPropertyAdaptorFactory::instance());
    PropertyFilters::registerFilter(PropertyFilter("QQuickItem", "anchors"));

    BindingAggregator::registerBindingProvider(std::unique_ptr<AbstractBindingProvider>(new QuickImplicitBindingDependencyProvider));
}
