/*
  quickscreengrabber.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "quickscreengrabber.h"
#include "core/enumutil.h"
#include "core/varianthandler.h"

#include <core/objectdataprovider.h>

#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QQuickWindow>
#include <QQuickRenderControl>

#ifndef QT_NO_OPENGL
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#endif

#include <private/qquickanchors_p.h>
#include <private/qquickitem_p.h>
#include <private/qquickwindow_p.h>

#include <private/qsgsoftwarerenderer_p.h>

#include <QQuickOpenGLUtils>

#include <algorithm>
#include <functional>
#include <cmath>

namespace GammaRay {

class QQuickItemPropertyCache
{
public:
    static const QQuickItemPropertyCache &getPropertyCache(QQuickItem *item)
    {
        static QHash<const QMetaObject *, QQuickItemPropertyCache> s_cache;
        const QMetaObject *meta = item->metaObject();
        const auto it = s_cache.constFind(meta);
        if (it != s_cache.cend())
            return *it;
        else
            return *s_cache.insert(meta, QQuickItemPropertyCache(meta));
    }

    explicit QQuickItemPropertyCache(const QMetaObject *meta)
        : background(property(meta, "background"))
        , contentItem(property(meta, "contentItem"))
        , padding(property(meta, "padding"))
    {
        if (padding.isValid()) {
            leftPadding = property(meta, "leftPadding");
            rightPadding = property(meta, "rightPadding");
            topPadding = property(meta, "topPadding");
            bottomPadding = property(meta, "bottomPadding");
        }
    }

    QMetaProperty background;
    QMetaProperty contentItem;
    QMetaProperty padding;
    QMetaProperty leftPadding;
    QMetaProperty rightPadding;
    QMetaProperty topPadding;
    QMetaProperty bottomPadding;

private:
    static inline QMetaProperty property(const QMetaObject *meta, const char *name)
    {
        return meta->property(meta->indexOfProperty(name));
    }
};

static QColor colorForItem(QQuickItem *item)
{
    // We need random colors, but we also want the item
    // to keep its random color during scene changes to avoid
    // flickering due to color change.
    static QHash<QQuickItem *, QColor> s_itemsColor;

    QColor color = s_itemsColor.value(item, QColor());

    if (!color.isValid()) {
        const auto h = qHash(ObjectDataProvider::shortTypeName(item));
        color = QColor::fromHsv(h % 360, 64 + h % 192, 128 + h % 128, 64);
        s_itemsColor[item] = color;
    }

    return color;
}

static bool quickItemZLessThan(QQuickItem *lhs, QQuickItem *rhs)
{
    return lhs->z() < rhs->z();
}

static bool quickItemZGreaterThan(QQuickItem *lhs, QQuickItem *rhs)
{
    return lhs->z() > rhs->z();
}

static QVector<QQuickItem *> findItemByClassName(const char *className, QQuickItem *parent,
                                                 const std::function<void(QQuickItem *)> &walker)
{
    Q_ASSERT(parent);
    QVector<QQuickItem *> items;

    if (!parent->window()) {
        return items;
    }

    if (parent != parent->window()->contentItem() && parent->inherits(className)) {
        items << parent;
        walker(parent);
    }

    QList<QQuickItem *> childItems = parent->childItems();
    // direct children of contentItem need to be sorted the over way so overlay is draw on top of the rest
    if (parent == parent->window()->contentItem()) {
        std::sort(childItems.begin(), childItems.end(), quickItemZGreaterThan);
    } else {
        std::sort(childItems.begin(), childItems.end(), quickItemZLessThan);
    }


    for (int i = childItems.size() - 1; i >= 0; --i) { // backwards to match z order
        const auto childItemChildItems = findItemByClassName(className, childItems.at(i), walker);

        if (!childItemChildItems.isEmpty())
            items << childItemChildItems;
    }

    return items;
}

static QQuickItem *toplevelItem(QQuickItem *item)
{
    Q_ASSERT(item);
    return item->window()->contentItem();
}

static QPointF itemPos(QQuickItem *item)
{
    Q_ASSERT(item);
    return { item->x(), item->y() };
}

static QSizeF itemSize(QQuickItem *item)
{
    Q_ASSERT(item);
    QSizeF size = QSizeF(item->width(), item->height());

    // Fallback to children rect if needed
    if (size.isNull()) {
        size = item->childrenRect().size();
    }

    return size;
}

static QRectF itemGeometry(QQuickItem *item)
{
    return QRectF(itemPos(item), itemSize(item));
}

static bool itemIsLayout(QQuickItem *item)
{
    Q_ASSERT(item);
    return item->inherits("QQuickLayout");
}
}

using namespace GammaRay;

ItemOrLayoutFacade::ItemOrLayoutFacade(QQuickItem *item)
    : m_object(item)
{
}

QQuickItem *ItemOrLayoutFacade::layout() const
{
    return isLayout() ? asLayout() : asItem();
}

QQuickItem *ItemOrLayoutFacade::item() const
{
    return isLayout() ? asLayout()->parentItem() : asItem();
}

QRectF ItemOrLayoutFacade::geometry() const
{
    return itemGeometry(isLayout() ? asLayout() : asItem());
}

bool ItemOrLayoutFacade::isVisible() const
{
    return item() ? item()->isVisible() : false;
}

QPointF ItemOrLayoutFacade::pos() const
{
    return isLayout() ? itemGeometry(asLayout()).topLeft() : QPoint(0, 0);
}

bool ItemOrLayoutFacade::isLayout() const
{
    return itemIsLayout(m_object);
}

std::unique_ptr<AbstractScreenGrabber> AbstractScreenGrabber::get(QQuickWindow *window)
{
    switch (graphicsApiFor(window)) {
#ifndef QT_NO_OPENGL
    case RenderInfo::OpenGL:
        return std::unique_ptr<AbstractScreenGrabber>(new OpenGLScreenGrabber(window));
#endif
    case RenderInfo::Software:
        return std::unique_ptr<AbstractScreenGrabber>(new SoftwareScreenGrabber(window));
    default:
        return std::unique_ptr<AbstractScreenGrabber>(new UnsupportedScreenGrabber(window));
    }
}

AbstractScreenGrabber::RenderInfo::GraphicsApi AbstractScreenGrabber::graphicsApiFor(QQuickWindow *window)
{
    if (!window) {
        return RenderInfo::Unknown;
    }

    return static_cast<RenderInfo::GraphicsApi>(window->rendererInterface()->graphicsApi());
}

AbstractScreenGrabber::AbstractScreenGrabber(QQuickWindow *window)
    : m_window(window)
    , m_currentToplevelItem(nullptr)
{
    qRegisterMetaType<GrabbedFrame>();

    placeOn(ItemOrLayoutFacade());
}

GammaRay::AbstractScreenGrabber::~AbstractScreenGrabber() = default;

QQuickWindow *AbstractScreenGrabber::window() const
{
    return m_window;
}

QuickDecorationsSettings AbstractScreenGrabber::settings() const
{
    return m_settings;
}

void AbstractScreenGrabber::setSettings(const QuickDecorationsSettings &settings)
{
    if (m_settings == settings)
        return;
    m_settings = settings;
    updateOverlay();
}

bool AbstractScreenGrabber::decorationsEnabled() const
{
    return m_decorationsEnabled;
}

void AbstractScreenGrabber::setDecorationsEnabled(bool enabled)
{
    if (m_decorationsEnabled == enabled)
        return;

    m_decorationsEnabled = enabled;
    updateOverlay();
}

void AbstractScreenGrabber::placeOn(const ItemOrLayoutFacade &item)
{
    if (item.isNull()) {
        if (!m_currentItem.isNull())
            disconnectItemChanges(m_currentItem.data());

        if (m_currentToplevelItem) {
            disconnectTopItemChanges(m_currentToplevelItem);
            // Ensure the overlay is cleared
            if (m_currentToplevelItem->window())
                m_currentToplevelItem->window()->update();
        }

        m_currentToplevelItem = nullptr;
        m_currentItem.clear();

        updateOverlay();
        return;
    }

    Q_ASSERT(item.item()->window() == m_window);

    if (!m_currentItem.isNull())
        disconnectItemChanges(m_currentItem.data());

    m_currentItem = item;

    QQuickItem *toplevel = toplevelItem(item.item());
    Q_ASSERT(toplevel);

    if (toplevel != m_currentToplevelItem) {
        if (m_currentToplevelItem) {
            disconnectTopItemChanges(m_currentToplevelItem);
            // Ensure the overlay is cleared
            m_currentToplevelItem->window()->update();
        }

        m_currentToplevelItem = toplevel;

        connectTopItemChanges(m_currentToplevelItem);
    }

    connectItemChanges(m_currentItem.data());

    updateOverlay();
}

QuickItemGeometry AbstractScreenGrabber::initFromItem(QQuickItem *item)
{
    QuickItemGeometry itemGeometry;

    if (!item) {
        Q_ASSERT(false);
        return itemGeometry;
    }

    QQuickItem *parent = item->parentItem();

    if (parent) {
        itemGeometry.itemRect = item->parentItem()->mapRectToScene(
            QRectF(item->x(), item->y(), item->width(), item->height()));
    } else {
        itemGeometry.itemRect = QRectF(0, 0, item->width(), item->height());
    }

    itemGeometry.boundingRect = item->mapRectToScene(item->boundingRect());
    itemGeometry.childrenRect = item->mapRectToScene(item->childrenRect());

    const QQuickItemPropertyCache &cache = QQuickItemPropertyCache::getPropertyCache(item);

    QQuickItem *background = cache.background.read(item).value<QQuickItem *>();
    if (background)
        itemGeometry.backgroundRect = background->mapRectToScene(background->boundingRect());
    QQuickItem *contentItem = cache.contentItem.read(item).value<QQuickItem *>();
    if (contentItem)
        itemGeometry.contentItemRect = contentItem->mapRectToScene(contentItem->boundingRect());
    itemGeometry.transformOriginPoint = item->mapToScene(item->transformOriginPoint());

    QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
    QQuickAnchors *anchors = itemPriv->_anchors;

    if (anchors) {
        QQuickAnchors::Anchors usedAnchors = anchors->usedAnchors();
        itemGeometry.left = ( bool )(usedAnchors & QQuickAnchors::LeftAnchor) || anchors->fill();
        itemGeometry.right = ( bool )(usedAnchors & QQuickAnchors::RightAnchor) || anchors->fill();
        itemGeometry.top = ( bool )(usedAnchors & QQuickAnchors::TopAnchor) || anchors->fill();
        itemGeometry.bottom = ( bool )(usedAnchors & QQuickAnchors::BottomAnchor) || anchors->fill();
        itemGeometry.baseline = ( bool )(usedAnchors & QQuickAnchors::BaselineAnchor);
        itemGeometry.horizontalCenter = ( bool )(usedAnchors & QQuickAnchors::HCenterAnchor)
            || anchors->centerIn();
        itemGeometry.verticalCenter = ( bool )(usedAnchors & QQuickAnchors::VCenterAnchor)
            || anchors->centerIn();
        itemGeometry.leftMargin = anchors->leftMargin();
        itemGeometry.rightMargin = anchors->rightMargin();
        itemGeometry.topMargin = anchors->topMargin();
        itemGeometry.bottomMargin = anchors->bottomMargin();
        itemGeometry.horizontalCenterOffset = anchors->horizontalCenterOffset();
        itemGeometry.verticalCenterOffset = anchors->verticalCenterOffset();
        itemGeometry.baselineOffset = anchors->baselineOffset();
        itemGeometry.margins = anchors->margins();
    }
    itemGeometry.x = item->x();
    itemGeometry.y = item->y();

    if (cache.padding.isValid()) {
        itemGeometry.padding = cache.padding.read(item).toReal();
        itemGeometry.leftPadding = cache.leftPadding.read(item).toReal();
        itemGeometry.rightPadding = cache.rightPadding.read(item).toReal();
        itemGeometry.topPadding = cache.topPadding.read(item).toReal();
        itemGeometry.bottomPadding = cache.bottomPadding.read(item).toReal();
    } else {
        itemGeometry.padding = qQNaN();
        itemGeometry.leftPadding = qQNaN();
        itemGeometry.rightPadding = qQNaN();
        itemGeometry.topPadding = qQNaN();
        itemGeometry.bottomPadding = qQNaN();
    }

    itemGeometry.transform = itemPriv->itemToWindowTransform();
    if (parent) {
        QQuickItemPrivate *parentPriv = QQuickItemPrivate::get(parent);
        itemGeometry.parentTransform = parentPriv->itemToWindowTransform();
    }

    itemGeometry.traceColor = colorForItem(item);
    itemGeometry.traceTypeName = ObjectDataProvider::shortTypeName(item);
    itemGeometry.traceName = ObjectDataProvider::name(item);

    return itemGeometry;
}

void AbstractScreenGrabber::gatherRenderInfo()
{
    // We are in the rendering thread at this point
    // And the gui thread is locked
    m_renderInfo.dpr = 1.0;
    // See QTBUG-53795
    m_renderInfo.dpr = m_window->effectiveDevicePixelRatio();
    m_renderInfo.windowSize = m_window->size();
    m_renderInfo.windowPosition = m_window->position();
    m_renderInfo.graphicsApi = static_cast<RenderInfo::GraphicsApi>(m_window->rendererInterface()->graphicsApi());

    m_grabbedFrame.itemsGeometry.clear();
    m_grabbedFrame.itemsGeometryRect = QRectF();

    if (m_window) {
        m_grabbedFrame.itemsGeometryRect = QRect(QPoint(), m_renderInfo.windowSize);

        if (m_settings.componentsTraces) {
            findItemByClassName("QQuickControl",
                                m_window->contentItem(),
                                [this](QQuickItem *item) {
                                    if (!item->isVisible())
                                        return;
                                    QuickItemGeometry itemGeometry = initFromItem(item);
                                    m_grabbedFrame.itemsGeometry << itemGeometry;
                                    m_grabbedFrame.itemsGeometryRect |= itemGeometry.itemRect | itemGeometry.childrenRect | itemGeometry.boundingRect;
                                });
        } else {
            QuickItemGeometry itemGeometry;
            if (!m_currentItem.isNull())
                itemGeometry = initFromItem(m_currentItem.data());
            m_grabbedFrame.itemsGeometry << itemGeometry;
            m_grabbedFrame.itemsGeometryRect |= itemGeometry.itemRect | itemGeometry.childrenRect | itemGeometry.boundingRect;
        }
    }
}

void AbstractScreenGrabber::doDrawDecorations(QPainter &painter)
{
    if (!m_decorationsEnabled)
        return;

    if (m_settings.componentsTraces) {
        const QuickDecorationsTracesInfo tracesInfo(m_settings,
                                                    m_grabbedFrame.itemsGeometry,
                                                    QRectF(QPointF(), m_renderInfo.windowSize),
                                                    1.0);
        QuickDecorationsDrawer drawer(QuickDecorationsDrawer::Traces, painter, tracesInfo);
        drawer.render();
    } else {
        const QuickDecorationsRenderInfo renderInfo(m_settings,
                                                    m_grabbedFrame.itemsGeometry.value(0),
                                                    QRectF(QPointF(), m_renderInfo.windowSize),
                                                    1.0);
        QuickDecorationsDrawer drawer(QuickDecorationsDrawer::Decorations, painter, renderInfo);
        drawer.render();
    }
}

void AbstractScreenGrabber::updateOverlay()
{
    if (m_window) {
        if (!m_currentItem.isNull())
            Q_ASSERT(m_currentItem.item()->window() == m_window);

        m_window->update();
    }
}

void AbstractScreenGrabber::itemParentChanged(QQuickItem *parent)
{
    Q_UNUSED(parent);
    if (!m_currentItem.isNull())
        placeOn(m_currentItem);
}

void AbstractScreenGrabber::itemWindowChanged(QQuickWindow *window)
{
    if (m_window == window) {
        if (!m_currentItem.isNull())
            placeOn(m_currentItem);
    } else {
        placeOn(ItemOrLayoutFacade());
    }
}

void AbstractScreenGrabber::connectItemChanges(QQuickItem *item) const
{
    connect(item, &QQuickItem::childrenRectChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::rotationChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::scaleChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::widthChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::heightChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::xChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::yChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::zChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::visibleChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::parentChanged, this, &AbstractScreenGrabber::itemParentChanged);
    connect(item, &QQuickItem::windowChanged, this, &AbstractScreenGrabber::itemWindowChanged);
}

void AbstractScreenGrabber::disconnectItemChanges(QQuickItem *item) const
{
    disconnect(item, &QQuickItem::childrenRectChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::rotationChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::scaleChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::widthChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::heightChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::xChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::yChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::zChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::visibleChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::parentChanged, this, &AbstractScreenGrabber::itemParentChanged);
    disconnect(item, &QQuickItem::windowChanged, this, &AbstractScreenGrabber::itemWindowChanged);
}

void AbstractScreenGrabber::connectTopItemChanges(QQuickItem *item) const
{
    connect(item, &QQuickItem::childrenRectChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::rotationChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::scaleChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::widthChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::heightChanged, this, &AbstractScreenGrabber::updateOverlay);
}

void AbstractScreenGrabber::disconnectTopItemChanges(QQuickItem *item) const
{
    disconnect(item, &QQuickItem::childrenRectChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::rotationChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::scaleChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::widthChanged, this, &AbstractScreenGrabber::updateOverlay);
    disconnect(item, &QQuickItem::heightChanged, this, &AbstractScreenGrabber::updateOverlay);
}

#ifndef QT_NO_OPENGL
OpenGLScreenGrabber::OpenGLScreenGrabber(QQuickWindow *window)
    : AbstractScreenGrabber(window)
    , m_isGrabbing(false)
{
    // Force DirectConnection else Auto lead to Queued which is not good.
    connect(m_window.data(), &QQuickWindow::afterSynchronizing,
            this, &OpenGLScreenGrabber::windowAfterSynchronizing, Qt::DirectConnection);
    connect(m_window.data(), &QQuickWindow::afterRendering,
            this, &OpenGLScreenGrabber::windowAfterRendering, Qt::DirectConnection);
}

OpenGLScreenGrabber::~OpenGLScreenGrabber() = default;

void OpenGLScreenGrabber::requestGrabWindow(const QRectF &userViewport)
{
    setGrabbingMode(true, userViewport);
}

void OpenGLScreenGrabber::setGrabbingMode(bool isGrabbing, const QRectF &userViewport)
{
    QMutexLocker locker(&m_mutex);

    if (m_isGrabbing == isGrabbing)
        return;

    m_isGrabbing = isGrabbing;
    m_userViewport = userViewport;

    emit grabberReadyChanged(!m_isGrabbing);

    if (m_isGrabbing)
        updateOverlay();
}

void OpenGLScreenGrabber::windowAfterSynchronizing()
{
    // We are in the rendering thread at this point
    // And the gui thread is locked
    gatherRenderInfo();
}

void OpenGLScreenGrabber::windowAfterRendering()
{
    QMutexLocker locker(&m_mutex);

    // We are in the rendering thread at this point
    // And the gui thread is NOT locked
    Q_ASSERT(QOpenGLContext::currentContext() == m_window->rendererInterface()->getResource(m_window, QSGRendererInterface::OpenGLContextResource));

    if (m_isGrabbing) {
        const auto window = QRectF(QPoint(0, 0), m_renderInfo.windowSize);
        const auto intersect = m_userViewport.isValid() ? window.intersected(m_userViewport) : window;

        // get viewport size
        QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
        int viewport[4]; // x, y, width, height
        glFuncs->glGetIntegerv(GL_VIEWPORT, viewport);
        int yOff = 0;
        int xOff = 0;

        // With Qt 6.4 all the content of the window (widgets + quick) is on the same surface, we need
        // to find the right x,y offsets to extract and show just our quick widget
        const bool isQQuickWidget = qstrcmp(m_window->metaObject()->className(), "QQuickWidgetOffscreenWindow") == 0;
        if (isQQuickWidget) {
#if QT_VERSION <= QT_VERSION_CHECK(6, 6, 0)
            xOff = m_renderInfo.windowPosition.x();
            const auto windowBottom = (m_renderInfo.windowPosition.y() + m_renderInfo.windowSize.height());
            const auto viewportHeight = viewport[3];
            yOff = viewportHeight - windowBottom;
#else
            const auto rc = QQuickWindowPrivate::get(m_window)->renderControl;
            QPoint offset;
            rc->renderWindow(&offset);

            xOff = offset.x();
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
            yOff = 0;
            xOff = static_cast<int>(std::floor(xOff * m_renderInfo.dpr));
#else
            const auto viewportHeight = viewport[3];
            const auto windowBottom = m_renderInfo.windowSize.height();
            yOff = viewportHeight - windowBottom - offset.y();
#endif
#endif
        }

        // readout parameters
        // when in doubt, round x and y to floor--> reads one pixel more
        const int x = static_cast<int>(std::floor(intersect.x() * m_renderInfo.dpr)) + xOff;
        // correct y for gpu-flipped textures being read from the bottom
        const int y = static_cast<int>(std::floor((m_renderInfo.windowSize.height() - intersect.height() - intersect.y()) * m_renderInfo.dpr)) + yOff;
        // when in doubt, round up w and h --> also reads one pixel more
        int w = static_cast<int>(std::ceil(intersect.width() * m_renderInfo.dpr));
        int h = static_cast<int>(std::ceil(intersect.height() * m_renderInfo.dpr));

        // cap to viewport size (which we can overshoot due to rounding errors in highdpi scaling)
        if (x + w > viewport[2])
            w = viewport[2] - x;
        if (y + h > viewport[3])
            h = viewport[3] - y;

        m_grabbedFrame.transform.reset();

        if (m_grabbedFrame.image.size() != QSize(w, h))
            m_grabbedFrame.image = QImage(w, h, QImage::Format_RGBA8888);

        glFuncs->glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, m_grabbedFrame.image.bits());

        // set transform to flip the read texture later, when displayed
        // Keep in mind that transforms are local coordinate (ie, not impacted by the device pixel ratio)
        m_grabbedFrame.transform.scale(1.0, -1.0);
        m_grabbedFrame.transform.translate(intersect.x(), -intersect.y() - intersect.height());
        m_grabbedFrame.image.setDevicePixelRatio(m_renderInfo.dpr);

        // Let emit the signal even if our image is possibly null, this way we make perfect ping/pong
        // requests making it easier to unit test.
        emit sceneGrabbed(m_grabbedFrame);
    }

    drawDecorations();

    QQuickOpenGLUtils::resetOpenGLState();

    if (m_isGrabbing) {
        locker.unlock();
        setGrabbingMode(false, QRectF());
    } else {
        emit sceneChanged();
    }
}

void OpenGLScreenGrabber::drawDecorations()
{
    // We are in the rendering thread at this point
    // And the gui thread is NOT locked
    QOpenGLPaintDevice device(m_renderInfo.windowSize * m_renderInfo.dpr);
    device.setDevicePixelRatio(m_renderInfo.dpr);
    QPainter p(&device);
    doDrawDecorations(p);
}
#endif

SoftwareScreenGrabber::SoftwareScreenGrabber(QQuickWindow *window)
    : AbstractScreenGrabber(window)
{
    connect(m_window.data(), &QQuickWindow::afterRendering,
            this, &SoftwareScreenGrabber::windowAfterRendering, Qt::DirectConnection);
    connect(m_window.data(), &QQuickWindow::beforeRendering,
            this, &SoftwareScreenGrabber::windowBeforeRendering, Qt::DirectConnection);
}

SoftwareScreenGrabber::~SoftwareScreenGrabber() = default;

void SoftwareScreenGrabber::updateOverlay()
{
    if (m_window) {
        if (!m_currentItem.isNull())
            Q_ASSERT(m_currentItem.item()->window() == m_window);

        auto renderer = softwareRenderer();
        if (renderer)
            renderer->markDirty();

        m_window->update();
    }
}

void SoftwareScreenGrabber::requestGrabWindow(const QRectF &userViewport)
{
    Q_UNUSED(userViewport);

    m_isGrabbing = true;
    qreal dpr = 1.0;
    // See QTBUG-53795
    dpr = m_window->effectiveDevicePixelRatio();

    m_grabbedFrame.image = QImage(m_window->size() * dpr, QImage::Format_ARGB32_Premultiplied);
    m_grabbedFrame.image.setDevicePixelRatio(dpr);
    m_grabbedFrame.image.fill(Qt::white);

    QQuickWindowPrivate *winPriv = QQuickWindowPrivate::get(m_window);
    QSGSoftwareRenderer *renderer = softwareRenderer();
    if (!renderer)
        return;

    QPaintDevice *regularRenderDevice = renderer->currentPaintDevice();
    renderer->setCurrentPaintDevice(&m_grabbedFrame.image);
    renderer->markDirty();
    winPriv->polishItems();
    winPriv->syncSceneGraph();
    winPriv->renderSceneGraph();
    renderer->setCurrentPaintDevice(regularRenderDevice);

    m_isGrabbing = false;

    emit sceneGrabbed(m_grabbedFrame);
}

void SoftwareScreenGrabber::drawDecorations()
{
    auto renderer = softwareRenderer();
    if (!renderer || !renderer->currentPaintDevice())
        return;
    QPainter p(renderer->currentPaintDevice());
    p.setClipRegion(renderer->flushRegion());
    doDrawDecorations(p);
}

void SoftwareScreenGrabber::windowBeforeRendering()
{
    QuickItemGeometry oldItemRect = m_grabbedFrame.itemsGeometry.size() ? m_grabbedFrame.itemsGeometry.front() : QuickItemGeometry(); // So far the vector never has more than one element...
    gatherRenderInfo();
    QuickItemGeometry newItemRect = m_grabbedFrame.itemsGeometry.size() ? m_grabbedFrame.itemsGeometry.front() : QuickItemGeometry();
    if (m_decorationsEnabled && newItemRect != oldItemRect) {
        // The item's scene coordinates can change unrecognizedly. If they do, we need a
        // full window repaint though, for the overlay to be correct.
        softwareRenderer()->markDirty();
    }
}

void SoftwareScreenGrabber::windowAfterRendering()
{
    if (!m_isGrabbing) {
        drawDecorations();
        emit sceneChanged();
    } else {
        m_isGrabbing = false;
    }
}

QSGSoftwareRenderer *SoftwareScreenGrabber::softwareRenderer() const
{
    QQuickWindowPrivate *winPriv = QQuickWindowPrivate::get(m_window);
    if (!winPriv)
        return nullptr;
    QSGSoftwareRenderer *softwareRenderer = dynamic_cast<QSGSoftwareRenderer *>(winPriv->renderer);
    return softwareRenderer;
}

UnsupportedScreenGrabber::UnsupportedScreenGrabber(QQuickWindow *window)
    : AbstractScreenGrabber(window)
{
}

UnsupportedScreenGrabber::~UnsupportedScreenGrabber()
{
}

void UnsupportedScreenGrabber::requestGrabWindow(const QRectF & /*userViewport*/)
{
    m_grabbedFrame.image = m_window->grabWindow();

    int alpha = 120;
    if (m_grabbedFrame.image.isNull()) {
        m_grabbedFrame.image = QImage(m_window->size() * m_window->effectiveDevicePixelRatio(), QImage::Format_ARGB32);
        m_grabbedFrame.image.fill(Qt::black);
        alpha = 200;
    }

    m_grabbedFrame.image.setDevicePixelRatio(m_window->effectiveDevicePixelRatio());

    QPainter p(&m_grabbedFrame.image);
    p.setRenderHint(QPainter::TextAntialiasing);
    QColor gray(Qt::black);
    gray.setAlpha(alpha);
    p.fillRect(QRect(QPoint {}, m_window->size()), gray);
    p.setPen(Qt::white);
    auto font = qApp->font();
    font.setPointSize(font.pointSize() + 1);
    p.setFont(font);
    QString backend = VariantHandler::displayString(QVariant::fromValue(m_window->graphicsApi()));
    QString txt = QLatin1String("%1 is not supported yet, please use the OpenGL (QSG_RHI_BACKEND=opengl) or Software backend (QT_QUICK_BACKEND=software)").arg(backend);
    p.drawText(QRect { QPoint(0, 0), m_window->size() }, Qt::AlignCenter | Qt::TextWordWrap, txt);

    emit sceneGrabbed(m_grabbedFrame);
}

void UnsupportedScreenGrabber::drawDecorations()
{
}

void UnsupportedScreenGrabber::updateOverlay()
{
}
