/*
  quickoverlay.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#include "quickscreengrabber.h"

#include <core/objectdataprovider.h>

#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QQuickWindow>

#ifndef QT_NO_OPENGL
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>
#endif

#include <private/qquickanchors_p.h>
#include <private/qquickitem_p.h>
#include <private/qquickwindow_p.h>

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
#include <private/qsgsoftwarerenderer_p.h>
#endif

#include <algorithm>
#include <functional>
#include <cmath>

namespace GammaRay {

class QQuickItemPropertyCache {
public:
    static const QQuickItemPropertyCache &getPropertyCache(QQuickItem *item)
    {
        static QHash<const QMetaObject*, QQuickItemPropertyCache> s_cache;
        const QMetaObject* meta = item->metaObject();
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
    static inline QMetaProperty property(const QMetaObject *meta, const char *name) {
         return meta->property(meta->indexOfProperty(name));
    }
};

// We need random colors, but we also want the item
// to keep its random color during scene changes to avoid
// flickering due to color change.
static QHash<QQuickItem *, QColor> s_itemsColor;

static QColor colorForItem(QQuickItem *item)
{
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
    return {item->x(), item->y()};
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

std::unique_ptr<AbstractScreenGrabber> AbstractScreenGrabber::get(QQuickWindow* window)
{
    switch (graphicsApiFor(window)) {
#ifndef QT_NO_OPENGL
        case RenderInfo::OpenGL:
            return std::unique_ptr<AbstractScreenGrabber>(new OpenGLScreenGrabber(window));
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
        case RenderInfo::Software:
            return std::unique_ptr<AbstractScreenGrabber>(new SoftwareScreenGrabber(window));
#endif
        default:
            return nullptr;
    }
}

AbstractScreenGrabber::RenderInfo::GraphicsApi AbstractScreenGrabber::graphicsApiFor(QQuickWindow* window)
{
    if (!window) {
        return RenderInfo::Unknown;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    return static_cast<RenderInfo::GraphicsApi>(window->rendererInterface()->graphicsApi());
#else
    return RenderInfo::OpenGL;
#endif
}

AbstractScreenGrabber::AbstractScreenGrabber(QQuickWindow *window)
    : m_window(window)
    , m_currentToplevelItem(nullptr)
{
    const QMetaObject *mo = metaObject();
    m_sceneChanged = mo->method(mo->indexOfSignal(QMetaObject::normalizedSignature("sceneChanged()")));
    Q_ASSERT(m_sceneChanged.methodIndex() != -1);
    m_sceneGrabbed = mo->method(mo->indexOfSignal(QMetaObject::normalizedSignature("sceneGrabbed(GammaRay::GrabbedFrame)")));
    Q_ASSERT(m_sceneGrabbed.methodIndex() != -1);

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
        itemGeometry.left = (bool)(usedAnchors &QQuickAnchors::LeftAnchor) || anchors->fill();
        itemGeometry.right = (bool)(usedAnchors &QQuickAnchors::RightAnchor) || anchors->fill();
        itemGeometry.top = (bool)(usedAnchors &QQuickAnchors::TopAnchor) || anchors->fill();
        itemGeometry.bottom = (bool)(usedAnchors &QQuickAnchors::BottomAnchor) || anchors->fill();
        itemGeometry.baseline = (bool)(usedAnchors & QQuickAnchors::BaselineAnchor);
        itemGeometry.horizontalCenter = (bool)(usedAnchors &QQuickAnchors::HCenterAnchor)
                || anchors->centerIn();
        itemGeometry.verticalCenter = (bool)(usedAnchors &QQuickAnchors::VCenterAnchor)
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    m_renderInfo.graphicsApi = static_cast<RenderInfo::GraphicsApi>(m_window->rendererInterface()->graphicsApi());
#else
    m_renderInfo.graphicsApi = RenderInfo::OpenGL;
#endif

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
                m_grabbedFrame.itemsGeometryRect |= itemGeometry.itemRect | itemGeometry.childrenRect |
                        itemGeometry.boundingRect;
            });
        } else {
            QuickItemGeometry itemGeometry;
            if (!m_currentItem.isNull())
                itemGeometry = initFromItem(m_currentItem.data());
            m_grabbedFrame.itemsGeometry << itemGeometry;
            m_grabbedFrame.itemsGeometryRect |= itemGeometry.itemRect | itemGeometry.childrenRect |
                    itemGeometry.boundingRect;
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

void AbstractScreenGrabber::connectItemChanges(QQuickItem *item)
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

void AbstractScreenGrabber::disconnectItemChanges(QQuickItem *item)
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

void AbstractScreenGrabber::connectTopItemChanges(QQuickItem *item)
{
    connect(item, &QQuickItem::childrenRectChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::rotationChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::scaleChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::widthChanged, this, &AbstractScreenGrabber::updateOverlay);
    connect(item, &QQuickItem::heightChanged, this, &AbstractScreenGrabber::updateOverlay);
}

void AbstractScreenGrabber::disconnectTopItemChanges(QQuickItem *item)
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
    Q_ASSERT(QOpenGLContext::currentContext() == m_window->openglContext());

    if (m_isGrabbing) {
        const auto window = QRectF(QPoint(0,0), m_renderInfo.windowSize);
        const auto intersect = m_userViewport.isValid() ? window.intersected(m_userViewport) : window;

        // readout parameters
        // when in doubt, round x and y to floor--> reads one pixel more
        const int x = static_cast<int>(std::floor(intersect.x() * m_renderInfo.dpr));
        // correct y for gpu-flipped textures being read from the bottom
        const int y = static_cast<int>(std::floor((m_renderInfo.windowSize.height() - intersect.height() - intersect.y()) * m_renderInfo.dpr));
        // when in doubt, round up w and h --> also reads one pixel more
        int w = static_cast<int>(std::ceil(intersect.width() * m_renderInfo.dpr));
        int h = static_cast<int>(std::ceil(intersect.height() * m_renderInfo.dpr));

        // cap to viewport size (which we can overshoot due to rounding errors in highdpi scaling)
        QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
        int viewport[4];
        glFuncs->glGetIntegerv(GL_VIEWPORT, viewport);
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
        m_grabbedFrame.transform.translate(intersect.x() , -intersect.y() - intersect.height());
        m_grabbedFrame.image.setDevicePixelRatio(m_renderInfo.dpr);

        // Let emit the signal even if our image is possibly null, this way we make perfect ping/pong
        // reuests making it easier to unit test.
        m_sceneGrabbed.invoke(this, Qt::QueuedConnection, Q_ARG(GammaRay::GrabbedFrame, m_grabbedFrame));
    }

    drawDecorations();

    m_window->resetOpenGLState();

    if (m_isGrabbing) {
        locker.unlock();
        setGrabbingMode(false, QRectF());
    } else {
        m_sceneChanged.invoke(this, Qt::QueuedConnection);
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

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
SoftwareScreenGrabber::SoftwareScreenGrabber(QQuickWindow* window)
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

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 3)
        auto renderer = softwareRenderer();
        if (renderer)
            renderer->markDirty();
#endif

        m_window->update();
    }
}

void SoftwareScreenGrabber::requestGrabWindow(const QRectF& userViewport)
{
    Q_UNUSED(userViewport);

    m_isGrabbing = true;
    qreal dpr = 1.0;
    // See QTBUG-53795
    dpr = m_window->effectiveDevicePixelRatio();

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 3)
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
    winPriv->renderSceneGraph(m_window->size());
    renderer->setCurrentPaintDevice(regularRenderDevice);
#else
    m_grabbedFrame.image = m_window->grabWindow();
    m_grabbedFrame.image.setDevicePixelRatio(dpr);
#endif

    m_isGrabbing = false;

    emit sceneGrabbed(m_grabbedFrame);
}

void SoftwareScreenGrabber::drawDecorations()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 3)
    auto renderer = softwareRenderer();
    if (!renderer)
        return;
    QPainter p(renderer->currentPaintDevice());
    p.setClipRegion(renderer->flushRegion());
    doDrawDecorations(p);
#endif
}

void SoftwareScreenGrabber::windowBeforeRendering()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 3)
    QuickItemGeometry oldItemRect = m_grabbedFrame.itemsGeometry.size() ? m_grabbedFrame.itemsGeometry.front() : QuickItemGeometry(); // So far the vector never has more than one element...
    gatherRenderInfo();
    QuickItemGeometry newItemRect = m_grabbedFrame.itemsGeometry.size() ? m_grabbedFrame.itemsGeometry.front() : QuickItemGeometry();
    if (m_decorationsEnabled && newItemRect != oldItemRect) {
        // The item's scene coordinates can change unrecognizedly. If they do, we need a
        // full window repaint though, for the overlay to be correct.
        softwareRenderer()->markDirty();
    }
#endif
}

void SoftwareScreenGrabber::windowAfterRendering()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 3)
    if (!m_isGrabbing) {
        drawDecorations();
        m_sceneChanged.invoke(this, Qt::QueuedConnection);
    } else {
        m_isGrabbing = false;
    }
#endif
}

QSGSoftwareRenderer *SoftwareScreenGrabber::softwareRenderer() const
{
    QQuickWindowPrivate *winPriv = QQuickWindowPrivate::get(m_window);
    if (!winPriv)
        return nullptr;
    QSGSoftwareRenderer *softwareRenderer = dynamic_cast<QSGSoftwareRenderer*>(winPriv->renderer);
    return softwareRenderer;
}
#endif


