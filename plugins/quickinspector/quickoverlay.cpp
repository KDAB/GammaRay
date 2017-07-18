/*
  quickoverlay.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickoverlay.h"

#include <core/objectdataprovider.h>

#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QQuickWindow>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPaintDevice>

#include <private/qquickanchors_p.h>
#include <private/qquickitem_p.h>

#include <functional>
#include <cmath>

namespace GammaRay {
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
                                                 std::function<void(QQuickItem *)> walker)
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
        qSort(childItems.begin(), childItems.end(), quickItemZGreaterThan);
    } else {
        qSort(childItems.begin(), childItems.end(), quickItemZLessThan);
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
    return QPointF(item->x(), item->y());
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

ItemOrLayoutFacade::ItemOrLayoutFacade()
    : m_object(nullptr)
{
}

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

QuickOverlay::QuickOverlay()
    : m_window(nullptr)
    , m_currentToplevelItem(nullptr)
    , m_isGrabbingMode(false)
    , m_decorationsEnabled(true)
{
    const QMetaObject *mo = metaObject();
    m_sceneGrabbed = mo->method(mo->indexOfSignal(QMetaObject::normalizedSignature("sceneGrabbed(GammaRay::GrabbedFrame)")));
    Q_ASSERT(m_sceneGrabbed.methodIndex() != -1);
    m_sceneChanged = mo->method(mo->indexOfSignal(QMetaObject::normalizedSignature("sceneChanged()")));
    Q_ASSERT(m_sceneChanged.methodIndex() != -1);
    m_setIsGrabbingMode = mo->method(mo->indexOfSlot(QMetaObject::normalizedSignature("setIsGrabbingMode(bool)")));
    Q_ASSERT(m_setIsGrabbingMode.methodIndex() != -1);

    qRegisterMetaType<GrabbedFrame>();
}

QQuickWindow *QuickOverlay::window() const
{
    return m_window;
}

void QuickOverlay::setWindow(QQuickWindow *window)
{
    if (m_window == window)
        return;

    if (m_window) {
        disconnect(m_window.data(), &QQuickWindow::afterSynchronizing,
                   this, &QuickOverlay::windowAfterSynchronizing);
        disconnect(m_window.data(), &QQuickWindow::afterRendering,
                   this, &QuickOverlay::windowAfterRendering);
    }

    placeOn(ItemOrLayoutFacade());
    m_window = window;

    if (m_window) {
        // Force DirectConnection else Auto lead to Queued which is not good.
        connect(m_window.data(), &QQuickWindow::afterSynchronizing,
                   this, &QuickOverlay::windowAfterSynchronizing, Qt::DirectConnection);
        connect(m_window.data(), &QQuickWindow::afterRendering,
                this, &QuickOverlay::windowAfterRendering, Qt::DirectConnection);
    }
}

QuickDecorationsSettings QuickOverlay::settings() const
{
    return m_settings;
}

void QuickOverlay::setSettings(const QuickDecorationsSettings &settings)
{
    if (m_settings == settings)
        return;
    m_settings = settings;
    updateOverlay();
}

bool QuickOverlay::decorationsEnabled() const
{
    return m_decorationsEnabled;
}

void QuickOverlay::setDecorationsEnabled(bool enabled)
{
    if (m_decorationsEnabled == enabled)
        return;

    m_decorationsEnabled = enabled;
    updateOverlay();
}

void QuickOverlay::placeOn(const ItemOrLayoutFacade &item)
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

void QuickOverlay::setIsGrabbingMode(bool isGrabbingMode)
{
    if (m_isGrabbingMode == isGrabbingMode)
        return;

    m_isGrabbingMode = isGrabbingMode;
    if (m_isGrabbingMode)
        updateOverlay();
}

QuickItemGeometry QuickOverlay::initFromItem(QQuickItem *item) const
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
    QQuickItem *background = item->property("background").value<QQuickItem *>();
    if (background)
        itemGeometry.backgroundRect = background->mapRectToScene(background->boundingRect());
    QQuickItem *contentItem = item->property("contentItem").value<QQuickItem *>();
    if (contentItem)
        itemGeometry.contentItemRect = contentItem->mapRectToScene(contentItem->boundingRect());
    itemGeometry.transformOriginPoint = item->mapToScene(item->transformOriginPoint());

    QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors *>();

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

    const QMetaObject *mo = item->metaObject();
    if (mo->property(mo->indexOfProperty("padding")).isValid()) {
        itemGeometry.padding = item->property("padding").toReal();
        itemGeometry.leftPadding = item->property("leftPadding").toReal();
        itemGeometry.rightPadding = item->property("rightPadding").toReal();
        itemGeometry.topPadding = item->property("topPadding").toReal();
        itemGeometry.bottomPadding = item->property("bottomPadding").toReal();
    } else {
        itemGeometry.padding = qQNaN();
        itemGeometry.leftPadding = qQNaN();
        itemGeometry.rightPadding = qQNaN();
        itemGeometry.topPadding = qQNaN();
        itemGeometry.bottomPadding = qQNaN();
    }

    QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
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

void QuickOverlay::windowAfterSynchronizing()
{
    // We are in the rendering thread at this point
    // And the gui thread is locked
    gatherRenderInfo();
}

void QuickOverlay::windowAfterRendering()
{
    // We are in the rendering thread at this point
    // And the gui thread is NOT locked

    Q_ASSERT(QOpenGLContext::currentContext() == m_window->openglContext());
    if (m_isGrabbingMode) {
        const auto window = QRectF(QPoint(0,0), m_renderInfo.windowSize);
        const auto intersect = m_userViewport.isValid() ? window.intersected(m_userViewport) : window ;

        // readout parameters
        // when in doubt, round x and y to floor--> reads one pixel more
        const int x = static_cast<int>(std::floor(intersect.x() * m_renderInfo.dpr));
        // correct y for gpu-flipped textures being read from the bottom
        const int y = static_cast<int>(std::floor((m_renderInfo.windowSize.height() - intersect.height() - intersect.y()) * m_renderInfo.dpr));
        // when in doubt, round up w and h --> also reads one pixel more
        const int w = static_cast<int>(std::ceil(intersect.width() * m_renderInfo.dpr));
        const int h = static_cast<int>(std::ceil(intersect.height() * m_renderInfo.dpr));

        m_grabbedFrame.transform.reset();

        if (m_grabbedFrame.image.size() != QSize(w, h))
            m_grabbedFrame.image = QImage(w, h, QImage::Format_RGBA8888);

        QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
        glFuncs->glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, m_grabbedFrame.image.bits());

        // set transform to flip the read texture later, when displayed
        m_grabbedFrame.transform.scale(1.0, -1.0);
        m_grabbedFrame.transform.translate(intersect.x() * m_renderInfo.dpr , -intersect.y() * m_renderInfo.dpr - h);
        m_grabbedFrame.image.setDevicePixelRatio(m_renderInfo.dpr);

        if (!m_grabbedFrame.image.isNull()) {
            m_sceneGrabbed.invoke(this, Qt::QueuedConnection, Q_ARG(GammaRay::GrabbedFrame, m_grabbedFrame));
        }
    }

    drawDecorations();

    m_window->resetOpenGLState();

    if (m_isGrabbingMode) {
        setIsGrabbingMode(false);
    } else {
        m_sceneChanged.invoke(this, Qt::QueuedConnection);
    }
}

void QuickOverlay::gatherRenderInfo()
{
    // We are in the rendering thread at this point
    // And the gui thread is locked
    m_renderInfo.dpr = 1.0;
    // See QTBUG-53795
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    m_renderInfo.dpr = m_window->effectiveDevicePixelRatio();
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_renderInfo.dpr = m_window->devicePixelRatio();
#endif
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

void QuickOverlay::drawDecorations()
{
    // We are in the rendering thread at this point
    // And the gui thread is NOT locked
    if (!m_decorationsEnabled)
        return;

    if (m_renderInfo.graphicsApi != RenderInfo::OpenGL)
        return; // TODO

    QOpenGLPaintDevice device(m_renderInfo.windowSize * m_renderInfo.dpr);
    device.setDevicePixelRatio(m_renderInfo.dpr);
    QPainter painter(&device);
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

void QuickOverlay::updateOverlay()
{
    if (m_window) {
        if (!m_currentItem.isNull())
            Q_ASSERT(m_currentItem.item()->window() == m_window);

        m_window->update();
    }
}

void QuickOverlay::itemParentChanged(QQuickItem *parent)
{
    Q_UNUSED(parent);
    if (!m_currentItem.isNull())
        placeOn(m_currentItem);
}

void QuickOverlay::itemWindowChanged(QQuickWindow *window)
{
    if (m_window == window) {
        if (!m_currentItem.isNull())
            placeOn(m_currentItem);
    } else {
        placeOn(ItemOrLayoutFacade());
    }
}

void QuickOverlay::connectItemChanges(QQuickItem *item)
{
    connect(item, &QQuickItem::childrenRectChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::rotationChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::scaleChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::widthChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::heightChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::xChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::yChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::zChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::visibleChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::parentChanged, this, &QuickOverlay::itemParentChanged);
    connect(item, &QQuickItem::windowChanged, this, &QuickOverlay::itemWindowChanged);
}

void QuickOverlay::disconnectItemChanges(QQuickItem *item)
{
    disconnect(item, &QQuickItem::childrenRectChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::rotationChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::scaleChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::widthChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::heightChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::xChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::yChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::zChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::visibleChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::parentChanged, this, &QuickOverlay::itemParentChanged);
    disconnect(item, &QQuickItem::windowChanged, this, &QuickOverlay::itemWindowChanged);
}

void QuickOverlay::connectTopItemChanges(QQuickItem *item)
{
    connect(item, &QQuickItem::childrenRectChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::rotationChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::scaleChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::widthChanged, this, &QuickOverlay::updateOverlay);
    connect(item, &QQuickItem::heightChanged, this, &QuickOverlay::updateOverlay);
}

void QuickOverlay::disconnectTopItemChanges(QQuickItem *item)
{
    disconnect(item, &QQuickItem::childrenRectChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::rotationChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::scaleChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::widthChanged, this, &QuickOverlay::updateOverlay);
    disconnect(item, &QQuickItem::heightChanged, this, &QuickOverlay::updateOverlay);
}

void QuickOverlay::requestGrabWindow(const QRectF &userViewport)
{
    if (m_isGrabbingMode)
        return;

    m_userViewport = userViewport;
    m_setIsGrabbingMode.invoke(this, Qt::QueuedConnection, Q_ARG(bool, true));
}
