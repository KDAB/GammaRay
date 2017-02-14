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

#include <QDebug>
#include <QEvent>
#include <QPainter>
#include <QQuickItem>
#include <QQuickWindow>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>

QT_BEGIN_NAMESPACE
extern Q_GUI_EXPORT QImage qt_gl_read_framebuffer(const QSize &size,
                                                  bool alpha_format, bool include_alpha);
QT_END_NAMESPACE

using namespace GammaRay;

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
    return item->inherits(QT_STRINGIFY(QT_PREPEND_NAMESPACE(QQuickLayout)));
}

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

// cppcheck-suppress uninitMemberVar
QuickOverlay::QuickOverlay()
    : m_window(nullptr)
    , m_currentToplevelItem(nullptr)
    , m_isGrabbingMode(false)
    , m_drawDecorations(true)
{
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
        disconnect(m_window, &QQuickWindow::afterRendering,
                   this, &QuickOverlay::windowAfterRendering);
    }

    placeOn(ItemOrLayoutFacade());
    m_window = window;

    if (m_window) {
        // Force DirectConnection else Auto lead to Queued which is not good.
        connect(m_window, &QQuickWindow::afterRendering,
                this, &QuickOverlay::windowAfterRendering, Qt::DirectConnection);
    }
}

QuickOverlaySettings QuickOverlay::settings() const
{
    return m_settings;
}

void QuickOverlay::setSettings(const QuickOverlaySettings &settings)
{
    if (m_settings == settings)
        return;
    m_settings = settings;
    updateOverlay();
}

void QuickOverlay::placeOn(ItemOrLayoutFacade item)
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
        m_effectiveGeometry = QuickItemGeometry();

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

bool QuickOverlay::drawDecorations() const
{
    return m_drawDecorations;
}

void QuickOverlay::setDrawDecorations(bool enabled)
{
    if (m_drawDecorations == enabled)
        return;

    m_drawDecorations = enabled;
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

void QuickOverlay::windowAfterRendering()
{
    // We are in the rendering thread at this point
    // And the gui thread is blocked
    Q_ASSERT(QOpenGLContext::currentContext() == m_window->openglContext());

    qreal dpr = 1.0;
    // See QTBUG-53795
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    dpr = m_window->effectiveDevicePixelRatio();
#elif QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    dpr = m_window->devicePixelRatio();
#endif

    if (m_isGrabbingMode) {
        QImage image = qt_gl_read_framebuffer(m_window->size() * dpr, false, false);
        image.setDevicePixelRatio(dpr);
        if (!image.isNull())
            QMetaObject::invokeMethod(this, "sceneGrabbed", Qt::QueuedConnection, Q_ARG(QImage, image));
    }

    drawDecorations(m_window->size(), dpr);

    m_window->resetOpenGLState();

    if (m_isGrabbingMode) {
        setIsGrabbingMode(false);
        return;
    }

    QMetaObject::invokeMethod(this, "sceneChanged", Qt::QueuedConnection);
}

void QuickOverlay::drawDecorations(const QSize &size, qreal dpr)
{
    if (!m_drawDecorations)
        return;

    QOpenGLPaintDevice device(size * dpr);
    device.setDevicePixelRatio(dpr);
    QPainter painter(&device);
    drawDecoration(&painter, RenderInfo(m_settings, m_effectiveGeometry, QRectF(QPointF(), size), 1.0));
}

void QuickOverlay::updateOverlay()
{
    if (!m_currentItem.isNull() && m_currentToplevelItem)
        m_effectiveGeometry.initFrom(m_currentItem.data());
    else
        m_effectiveGeometry = QuickItemGeometry();

    if (m_window) {
        if (!m_currentItem.isNull()) {
            Q_ASSERT(m_currentItem.item()->window() == m_window);
}

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

void QuickOverlay::drawDecoration(QPainter *painter, const RenderInfo &renderInfo)
{
    if (!renderInfo.itemGeometry.valid)
        return;

    const QuickItemGeometry &itemGeometry(renderInfo.itemGeometry);
    const qreal &zoom(renderInfo.zoom);
    DrawTextInfoList texts;

    painter->save();

    // bounding box
    painter->setPen(renderInfo.settings.boundingRectColor);
    painter->setBrush(renderInfo.settings.boundingRectBrush);
    painter->drawRect(itemGeometry.boundingRect);

    // original geometry
    if (itemGeometry.itemRect != itemGeometry.boundingRect) {
        painter->setPen(renderInfo.settings.geometryRectColor);
        painter->setBrush(renderInfo.settings.geometryRectBrush);
        painter->drawRect(itemGeometry.itemRect);
    }

    // children rect
    if (itemGeometry.itemRect != itemGeometry.boundingRect &&
        itemGeometry.transform.isIdentity()) {
        // If this item is transformed the children rect will be painted wrongly,
        // so for now skip painting it.
        painter->setPen(renderInfo.settings.childrenRectColor);
        painter->setBrush(renderInfo.settings.childrenRectBrush);
        painter->drawRect(itemGeometry.childrenRect);
    }

    // transform origin
    if (itemGeometry.itemRect != itemGeometry.boundingRect) {
        painter->setPen(renderInfo.settings.transformOriginColor);
        painter->drawEllipse(itemGeometry.transformOriginPoint, 2.5, 2.5);
        painter->drawLine(itemGeometry.transformOriginPoint - QPointF(0, 6),
                          itemGeometry.transformOriginPoint + QPointF(0, 6));
        painter->drawLine(itemGeometry.transformOriginPoint - QPointF(6, 0),
                          itemGeometry.transformOriginPoint + QPointF(6, 0));
    }

    // x and y values
    painter->setPen(renderInfo.settings.coordinatesColor);
    if (!itemGeometry.left &&
        !itemGeometry.horizontalCenter &&
        !itemGeometry.right &&
        itemGeometry.x != 0) {
        QPointF parentEnd = (QPointF(itemGeometry.itemRect.x() - itemGeometry.x,
                           itemGeometry.itemRect.y()));
        QPointF itemEnd = itemGeometry.itemRect.topLeft();
        drawArrow(painter, parentEnd, itemEnd);
        texts << DrawTextInfo(
                     painter->pen(),
                     QRectF(parentEnd.x(), parentEnd.y() + 10, itemEnd.x() - parentEnd.x(), 50),
                     QStringLiteral("x: %1px").arg(itemGeometry.x / zoom),
                     Qt::AlignHCenter | Qt::TextDontClip
                     );
    }
    if (!itemGeometry.top &&
        !itemGeometry.verticalCenter &&
        !itemGeometry.bottom &&
        !itemGeometry.baseline &&
        itemGeometry.y != 0) {
        QPointF parentEnd = (QPointF(itemGeometry.itemRect.x(),
                           itemGeometry.itemRect.y() - itemGeometry.y));
        QPointF itemEnd = itemGeometry.itemRect.topLeft();
        drawArrow(painter, parentEnd, itemEnd);
        texts << DrawTextInfo(
                     painter->pen(),
                     QRectF(parentEnd.x() + 10, parentEnd.y(), 100, itemEnd.y() - parentEnd.y()),
                     QStringLiteral("y: %1px").arg(itemGeometry.y / zoom),
                     Qt::AlignVCenter | Qt::TextDontClip
                     );
    }

    // anchors
    painter->setPen(renderInfo.settings.marginsColor);

    if (itemGeometry.left) {
        drawHorizontalAnchor(painter, renderInfo, itemGeometry.itemRect.left(),
                             itemGeometry.leftMargin);
        texts << drawHorizontalAnchorLabel(painter, renderInfo, itemGeometry.itemRect.left(),
                                           itemGeometry.leftMargin,
                                           QStringLiteral("%1px").arg(itemGeometry.leftMargin / zoom),
                                           Qt::AlignBottom | Qt::AlignHCenter);
    }

    if (itemGeometry.horizontalCenter) {
        drawHorizontalAnchor(painter, renderInfo,
                             (itemGeometry.itemRect.left() + itemGeometry.itemRect.right()) / 2,
                             itemGeometry.horizontalCenterOffset);
        texts << drawHorizontalAnchorLabel(painter, renderInfo,
                             (itemGeometry.itemRect.left() + itemGeometry.itemRect.right()) / 2,
                             itemGeometry.horizontalCenterOffset,
                             QStringLiteral("offset: %1px").arg(itemGeometry.horizontalCenterOffset / zoom),
                             Qt::AlignBottom | Qt::AlignHCenter);
    }

    if (itemGeometry.right) {
        drawHorizontalAnchor(painter, renderInfo, itemGeometry.itemRect.right(),
                             -itemGeometry.rightMargin);
        texts << drawHorizontalAnchorLabel(painter, renderInfo, itemGeometry.itemRect.right(),
                             -itemGeometry.rightMargin,
                             QStringLiteral("%1px").arg(itemGeometry.rightMargin / zoom),
                             Qt::AlignTop | Qt::AlignHCenter);
    }

    if (itemGeometry.top) {
        drawVerticalAnchor(painter, renderInfo, itemGeometry.itemRect.top(),
                           itemGeometry.topMargin);
        texts << drawVerticalAnchorLabel(painter, renderInfo, itemGeometry.itemRect.top(),
                           itemGeometry.topMargin,
                           QStringLiteral("%1px").arg(itemGeometry.topMargin / zoom),
                           Qt::AlignVCenter | Qt::AlignRight);
    }

    if (itemGeometry.verticalCenter) {
        drawVerticalAnchor(painter, renderInfo, 
                           (itemGeometry.itemRect.top() + itemGeometry.itemRect.bottom()) / 2,
                           itemGeometry.verticalCenterOffset);
        texts << drawVerticalAnchorLabel(painter, renderInfo,
                           (itemGeometry.itemRect.top() + itemGeometry.itemRect.bottom()) / 2,
                           itemGeometry.verticalCenterOffset,
                           QStringLiteral("offset: %1px").arg(itemGeometry.verticalCenterOffset / zoom),
                           Qt::AlignVCenter | Qt::AlignRight);
    }

    if (itemGeometry.bottom) {
        drawVerticalAnchor(painter, renderInfo, itemGeometry.itemRect.bottom(),
                           -itemGeometry.bottomMargin);
        texts << drawVerticalAnchorLabel(painter, renderInfo, itemGeometry.itemRect.bottom(),
                           -itemGeometry.bottomMargin,
                           QStringLiteral("%1px").arg(itemGeometry.bottomMargin / zoom),
                           Qt::AlignVCenter | Qt::AlignLeft);
    }

    if (itemGeometry.baseline) {
        drawVerticalAnchor(painter, renderInfo, itemGeometry.itemRect.top(),
                           itemGeometry.baselineOffset);
        texts << drawVerticalAnchorLabel(painter, renderInfo,
                                         itemGeometry.itemRect.top(), itemGeometry.baselineOffset,
                           QStringLiteral("offset: %1px").arg(itemGeometry.baselineOffset / zoom),
                           Qt::AlignVCenter | Qt::AlignLeft);
    }

    // padding
    painter->setPen(renderInfo.settings.paddingColor);

    if (!qIsNaN(itemGeometry.leftPadding)) {
        drawHorizontalAnchor(painter, renderInfo, itemGeometry.itemRect.left(),
                             -itemGeometry.leftPadding);
        texts << drawHorizontalAnchorLabel(painter, renderInfo, itemGeometry.itemRect.left(),
                             -itemGeometry.leftPadding,
                             QStringLiteral("%1px").arg(itemGeometry.leftPadding / zoom),
                             Qt::AlignTop | Qt::AlignHCenter);
    }

    if (!qIsNaN(itemGeometry.rightPadding)) {
        drawHorizontalAnchor(painter, renderInfo, itemGeometry.itemRect.right(),
                             itemGeometry.rightPadding);
        texts << drawHorizontalAnchorLabel(painter, renderInfo, itemGeometry.itemRect.right(),
                             itemGeometry.rightPadding,
                             QStringLiteral("%1px").arg(itemGeometry.rightPadding / zoom),
                             Qt::AlignBottom | Qt::AlignHCenter);
    }

    if (!qIsNaN(itemGeometry.topPadding)) {
        drawVerticalAnchor(painter, renderInfo, itemGeometry.itemRect.top(),
                           -itemGeometry.topPadding);
        texts << drawVerticalAnchorLabel(painter, renderInfo, itemGeometry.itemRect.top(),
                           -itemGeometry.topPadding,
                           QStringLiteral("%1px").arg(itemGeometry.topPadding / zoom),
                           Qt::AlignVCenter | Qt::AlignLeft);
    }

    if (!qIsNaN(itemGeometry.bottomPadding)) {
        drawVerticalAnchor(painter, renderInfo, itemGeometry.itemRect.bottom(),
                           itemGeometry.bottomPadding);
        texts << drawVerticalAnchorLabel(painter, renderInfo, itemGeometry.itemRect.bottom(),
                           itemGeometry.bottomPadding,
                           QStringLiteral("%1px").arg(itemGeometry.bottomPadding / zoom),
                           Qt::AlignVCenter | Qt::AlignRight);
    }

    // Finally draw texts over the traced rectangles and lines
    // This make sure texts are always readable
    foreach (const auto &t, texts) {
        if (t.label.isEmpty())
            continue;
        painter->setPen(t.pen);
        painter->drawText(t.rect, t.align, t.label);
    }

    painter->restore();
}

void QuickOverlay::requestGrabWindow()
{
    if (m_isGrabbingMode)
        return;

    setIsGrabbingMode(true);
}

void QuickOverlay::drawArrow(QPainter *p, QPointF first, QPointF second)
{
    p->drawLine(first, second);
    QPointF vector(second - first);
    QMatrix m;
    m.rotate(30);
    QVector2D v1 = QVector2D(m.map(vector)).normalized() * 10;
    m.rotate(-60);
    QVector2D v2 = QVector2D(m.map(vector)).normalized() * 10;
    p->drawLine(first, first + v1.toPointF());
    p->drawLine(first, first + v2.toPointF());
    p->drawLine(second, second - v1.toPointF());
    p->drawLine(second, second - v2.toPointF());
}

void QuickOverlay::drawAnchor(QPainter *p, const RenderInfo &renderInfo,
                              Qt::Orientation orientation, qreal ownAnchorLine, qreal offset)
{
    const QuickItemGeometry &itemGeometry(renderInfo.itemGeometry);
    const QRectF &viewRect(renderInfo.viewRect);
    const qreal &zoom(renderInfo.zoom);

    p->save();

    const qreal foreignAnchorLine = ownAnchorLine - offset;

    // Arrow
    if (offset) {
        if (orientation == Qt::Horizontal) {
            drawArrow(p,
                      QPointF(foreignAnchorLine,
                              (itemGeometry.itemRect.top() + itemGeometry.itemRect.bottom()) / 2),
                      QPointF(ownAnchorLine,
                              (itemGeometry.itemRect.top() + itemGeometry.itemRect.bottom()) / 2));
        } else {
            drawArrow(p,
                      QPointF((itemGeometry.itemRect.left() + itemGeometry.itemRect.right()) / 2,
                              foreignAnchorLine),
                      QPointF((itemGeometry.itemRect.left() + itemGeometry.itemRect.right()) / 2,
                              ownAnchorLine));
        }
    }

    // Own Anchor line
    QPen pen(p->pen());
    pen.setWidth(2);
    p->setPen(pen);
    if (orientation == Qt::Horizontal)
        p->drawLine(ownAnchorLine,
                    itemGeometry.itemRect.top(), ownAnchorLine,
                    itemGeometry.itemRect.bottom());
    else
        p->drawLine(
                    itemGeometry.itemRect.left(), ownAnchorLine,
                    itemGeometry.itemRect.right(), ownAnchorLine);

    // Foreign Anchor line
    pen.setStyle(Qt::DotLine);
    p->setPen(pen);
    if (orientation == Qt::Horizontal)
        p->drawLine(foreignAnchorLine, 0, foreignAnchorLine, viewRect.height() * zoom);
    else
        p->drawLine(0, foreignAnchorLine, viewRect.width() * zoom, foreignAnchorLine);

    p->restore();
}

void QuickOverlay::drawVerticalAnchor(QPainter *p, const RenderInfo &renderInfo, qreal ownAnchorLine,
                                      qreal offset)
{
    drawAnchor(p, renderInfo, Qt::Vertical, ownAnchorLine, offset);
}

void QuickOverlay::drawHorizontalAnchor(QPainter *p, const RenderInfo &renderInfo, qreal ownAnchorLine,
                                        qreal offset)
{
    drawAnchor(p, renderInfo, Qt::Horizontal, ownAnchorLine, offset);
}

QuickOverlay::DrawTextInfo QuickOverlay::drawAnchorLabel(QPainter *p, const QuickOverlay::RenderInfo &renderInfo,
        Qt::Orientation orientation, qreal ownAnchorLine, qreal offset, const QString &label, Qt::Alignment align)
{
    const QuickItemGeometry &itemGeometry(renderInfo.itemGeometry);

    if (align.testFlag(Qt::AlignCenter)) {
        qWarning("%s: You can not use Qt::AlignCenter!", Q_FUNC_INFO);
        return {};
    }

    if (align.testFlag(Qt::AlignJustify)) {
        qWarning("%s: You can not use Qt::AlignJustify!", Q_FUNC_INFO);
        return {};
    }

    if (align.testFlag(Qt::AlignBaseline)) {
        qWarning("%s: You can not use Qt::AlignBaseline!", Q_FUNC_INFO);
        return {};
    }

    if (offset) {
        const qreal foreignAnchorLine = ownAnchorLine - offset;
        const int margin = 10;

        if (orientation == Qt::Horizontal) {
            QRectF rect(p->fontMetrics().boundingRect(label));

            QPointF center(foreignAnchorLine + ((ownAnchorLine - foreignAnchorLine) / 2),
                           itemGeometry.itemRect.center().y());

            if (align.testFlag(Qt::AlignLeft)) {
                rect.moveRight(center.x());

                if (align.testFlag(Qt::AlignVCenter)) {
                    rect.moveRight(foreignAnchorLine - margin);
                }
            } else if (align.testFlag(Qt::AlignRight)) {
                rect.moveLeft(center.x());

                if (align.testFlag(Qt::AlignVCenter)) {
                    rect.moveLeft(foreignAnchorLine + offset + margin);
                }
            } else if (align.testFlag(Qt::AlignHCenter)) {
                rect.moveCenter(QPointF(center.x(), rect.center().y()));
            }

            if (align.testFlag(Qt::AlignTop)) {
                rect.moveBottom(center.y() - margin);
            } else if (align.testFlag(Qt::AlignBottom)) {
                rect.moveTop(center.y() + margin);
            } else if (align.testFlag(Qt::AlignVCenter)) {
                rect.moveCenter(QPointF(rect.center().x(), center.y()));
            }

            return {
                p->pen(),
                        rect,
                        label
            };
        } else {
            QRectF rect(p->fontMetrics().boundingRect(label));

            QPointF center(itemGeometry.itemRect.center().x(),
                           foreignAnchorLine + ((ownAnchorLine - foreignAnchorLine) / 2));

            if (align.testFlag(Qt::AlignLeft)) {
                rect.moveRight(center.x() - margin);
            } else if (align.testFlag(Qt::AlignRight)) {
                rect.moveLeft(center.x() + margin);
            } else if (align.testFlag(Qt::AlignHCenter)) {
                rect.moveCenter(QPointF(center.x(), rect.center().y()));
            }

            if (align.testFlag(Qt::AlignTop)) {
                rect.moveBottom(center.y());
            } else if (align.testFlag(Qt::AlignBottom)) {
                rect.moveTop(center.y());
            } else if (align.testFlag(Qt::AlignVCenter)) {
                rect.moveCenter(QPointF(rect.center().x(), center.y()));
            }

            return {
                p->pen(),
                        rect,
                        label
            };
        }
    }

    return {};
}

QuickOverlay::DrawTextInfo QuickOverlay::drawHorizontalAnchorLabel(QPainter *p, const QuickOverlay::RenderInfo &renderInfo,
                                              qreal ownAnchorLine, qreal offset, const QString &label, Qt::Alignment align)
{
    return drawAnchorLabel(p, renderInfo, Qt::Horizontal, ownAnchorLine, offset, label, align);
}

QuickOverlay::DrawTextInfo QuickOverlay::drawVerticalAnchorLabel(QPainter *p, const QuickOverlay::RenderInfo &renderInfo,
                                            qreal ownAnchorLine, qreal offset, const QString &label, Qt::Alignment align)
{
    return drawAnchorLabel(p, renderInfo, Qt::Vertical, ownAnchorLine, offset, label, align);
}

QDataStream &GammaRay::operator<<(QDataStream &stream, const GammaRay::QuickOverlaySettings &settings)
{
    stream
            << settings.boundingRectColor
            << settings.boundingRectBrush
            << settings.geometryRectColor
            << settings.geometryRectBrush
            << settings.childrenRectColor
            << settings.childrenRectBrush
            << settings.transformOriginColor
            << settings.coordinatesColor
            << settings.marginsColor
            << settings.paddingColor
    ;

    return stream;
}

QDataStream &GammaRay::operator>>(QDataStream &stream, GammaRay::QuickOverlaySettings &settings)
{
    stream
            >> settings.boundingRectColor
            >> settings.boundingRectBrush
            >> settings.geometryRectColor
            >> settings.geometryRectBrush
            >> settings.childrenRectColor
            >> settings.childrenRectBrush
            >> settings.transformOriginColor
            >> settings.coordinatesColor
            >> settings.marginsColor
            >> settings.paddingColor
    ;

    return stream;
}
