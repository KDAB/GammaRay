/*
  quickdecorationsdrawer.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickdecorationsdrawer.h"

#include <compat/qasconst.h>

#include <QDebug>
#include <QPainter>
#include <QVector2D>

#ifndef GL_BGRA
#define GL_BGRA GL_BGRA_EXT
#endif

using namespace GammaRay;

QuickDecorationsDrawer::QuickDecorationsDrawer(QuickDecorationsDrawer::Type type,
                                               QPainter &painter,
                                               const QuickDecorationsBaseRenderInfo &renderInfo)
    : m_type(type)
    , m_renderInfo(&renderInfo)
    , m_painter(&painter)
{
    Q_ASSERT(m_painter);
    Q_ASSERT(m_renderInfo);
}

void QuickDecorationsDrawer::render()
{
    // Draw the grid if needed
    drawGrid();

    switch (m_type) {
    case QuickDecorationsDrawer::Decorations: {
        drawDecorations();
        break;
    }

    case QuickDecorationsDrawer::Traces: {
        drawTraces();
        break;
    }
    }
}

void QuickDecorationsDrawer::drawDecorations()
{
    QuickItemGeometry itemGeometry(this->itemGeometry());

    if (!itemGeometry.isValid())
        return;

    const qreal &zoom(m_renderInfo->zoom);
    DrawTextInfoList texts;

    itemGeometry.scaleTo(zoom);

    m_painter->save();

    // bounding box
    if (itemGeometry.boundingRect.isValid()) {
        m_painter->setPen(m_renderInfo->settings.boundingRectColor);
        m_painter->setBrush(m_renderInfo->settings.boundingRectBrush);
        m_painter->drawRect(itemGeometry.boundingRect);
    }

    // original geometry
    if (itemGeometry.itemRect.isValid() && itemGeometry.itemRect != itemGeometry.boundingRect) {
        m_painter->setPen(m_renderInfo->settings.geometryRectColor);
        m_painter->setBrush(m_renderInfo->settings.geometryRectBrush);
        m_painter->drawRect(itemGeometry.itemRect);
    }

    // children rect
    if (itemGeometry.childrenRect.isValid() && itemGeometry.itemRect != itemGeometry.boundingRect &&
            itemGeometry.transform.isIdentity()) {
        // If this item is transformed the children rect will be painted wrongly,
        // so for now skip painting it.
        m_painter->setPen(m_renderInfo->settings.childrenRectColor);
        m_painter->setBrush(m_renderInfo->settings.childrenRectBrush);
        m_painter->drawRect(itemGeometry.childrenRect);
    }

    // transform origin
    if (itemGeometry.itemRect != itemGeometry.boundingRect) {
        m_painter->setPen(m_renderInfo->settings.transformOriginColor);
        m_painter->drawEllipse(itemGeometry.transformOriginPoint, 2.5, 2.5);
        m_painter->drawLine(itemGeometry.transformOriginPoint - QPointF(0, 6),
                          itemGeometry.transformOriginPoint + QPointF(0, 6));
        m_painter->drawLine(itemGeometry.transformOriginPoint - QPointF(6, 0),
                          itemGeometry.transformOriginPoint + QPointF(6, 0));
    }

    // x and y values
    m_painter->setPen(m_renderInfo->settings.coordinatesColor);
    if (!itemGeometry.left &&
            !itemGeometry.horizontalCenter &&
            !itemGeometry.right &&
            itemGeometry.x != 0) {
        QPointF parentEnd = (QPointF(itemGeometry.itemRect.x() - itemGeometry.x,
                                     itemGeometry.itemRect.y()));
        QPointF itemEnd = itemGeometry.itemRect.topLeft();
        drawArrow(parentEnd, itemEnd);
        texts << DrawTextInfo(m_painter->pen(),
                              QRectF(parentEnd.x(), parentEnd.y() + 10, itemEnd.x() - parentEnd.x(), 50),
                              QStringLiteral("x: %1px").arg(itemGeometry.x / zoom),
                              Qt::AlignHCenter | Qt::TextDontClip);
    }
    if (!itemGeometry.top &&
            !itemGeometry.verticalCenter &&
            !itemGeometry.bottom &&
            !itemGeometry.baseline &&
            itemGeometry.y != 0) {
        QPointF parentEnd = (QPointF(itemGeometry.itemRect.x(),
                                     itemGeometry.itemRect.y() - itemGeometry.y));
        QPointF itemEnd = itemGeometry.itemRect.topLeft();
        drawArrow(parentEnd, itemEnd);
        texts << DrawTextInfo(m_painter->pen(),
                              QRectF(parentEnd.x() + 10, parentEnd.y(), 100, itemEnd.y() - parentEnd.y()),
                              QStringLiteral("y: %1px").arg(itemGeometry.y / zoom),
                              Qt::AlignVCenter | Qt::TextDontClip);
    }

    // anchors
    m_painter->setPen(m_renderInfo->settings.marginsColor);

    if (itemGeometry.left) {
        drawHorizontalAnchor(itemGeometry, itemGeometry.itemRect.left(),
                             itemGeometry.leftMargin);
        texts << drawHorizontalAnchorLabel(itemGeometry,
                                           itemGeometry.itemRect.left(),
                                           itemGeometry.leftMargin,
                                           QStringLiteral("%1px").arg(itemGeometry.leftMargin / zoom),
                                           Qt::AlignBottom | Qt::AlignHCenter);
    }

    if (itemGeometry.horizontalCenter) {
        drawHorizontalAnchor(itemGeometry,
                             (itemGeometry.itemRect.left() + itemGeometry.itemRect.right()) / 2,
                             itemGeometry.horizontalCenterOffset);
        texts << drawHorizontalAnchorLabel(itemGeometry,
                                           (itemGeometry.itemRect.left() + itemGeometry.itemRect.right()) / 2,
                                           itemGeometry.horizontalCenterOffset,
                                           QStringLiteral("offset: %1px").arg(itemGeometry.horizontalCenterOffset / zoom),
                                           Qt::AlignBottom | Qt::AlignHCenter);
    }

    if (itemGeometry.right) {
        drawHorizontalAnchor(itemGeometry,
                             itemGeometry.itemRect.right(),
                             -itemGeometry.rightMargin);
        texts << drawHorizontalAnchorLabel(itemGeometry,
                                           itemGeometry.itemRect.right(),
                                           -itemGeometry.rightMargin,
                                           QStringLiteral("%1px").arg(itemGeometry.rightMargin / zoom),
                                           Qt::AlignTop | Qt::AlignHCenter);
    }

    if (itemGeometry.top) {
        drawVerticalAnchor(itemGeometry,
                           itemGeometry.itemRect.top(),
                           itemGeometry.topMargin);
        texts << drawVerticalAnchorLabel(itemGeometry,
                                         itemGeometry.itemRect.top(),
                                         itemGeometry.topMargin,
                                         QStringLiteral("%1px").arg(itemGeometry.topMargin / zoom),
                                         Qt::AlignVCenter | Qt::AlignRight);
    }

    if (itemGeometry.verticalCenter) {
        drawVerticalAnchor(itemGeometry,
                           (itemGeometry.itemRect.top() + itemGeometry.itemRect.bottom()) / 2,
                           itemGeometry.verticalCenterOffset);
        texts << drawVerticalAnchorLabel(itemGeometry,
                                         (itemGeometry.itemRect.top() + itemGeometry.itemRect.bottom()) / 2,
                                         itemGeometry.verticalCenterOffset,
                                         QStringLiteral("offset: %1px").arg(itemGeometry.verticalCenterOffset / zoom),
                                         Qt::AlignVCenter | Qt::AlignRight);
    }

    if (itemGeometry.bottom) {
        drawVerticalAnchor(itemGeometry,
                           itemGeometry.itemRect.bottom(),
                           -itemGeometry.bottomMargin);
        texts << drawVerticalAnchorLabel(itemGeometry,
                                         itemGeometry.itemRect.bottom(),
                                         -itemGeometry.bottomMargin,
                                         QStringLiteral("%1px").arg(itemGeometry.bottomMargin / zoom),
                                         Qt::AlignVCenter | Qt::AlignLeft);
    }

    if (itemGeometry.baseline) {
        drawVerticalAnchor(itemGeometry,
                           itemGeometry.itemRect.top(),
                           itemGeometry.baselineOffset);
        texts << drawVerticalAnchorLabel(itemGeometry,
                                         itemGeometry.itemRect.top(), itemGeometry.baselineOffset,
                                         QStringLiteral("offset: %1px").arg(itemGeometry.baselineOffset / zoom),
                                         Qt::AlignVCenter | Qt::AlignLeft);
    }

    // padding
    m_painter->setPen(m_renderInfo->settings.paddingColor);

    if (!qIsNaN(itemGeometry.leftPadding)) {
        drawHorizontalAnchor(itemGeometry,
                             itemGeometry.itemRect.left(),
                             -itemGeometry.leftPadding);
        texts << drawHorizontalAnchorLabel(itemGeometry,
                                           itemGeometry.itemRect.left(),
                                           -itemGeometry.leftPadding,
                                           QStringLiteral("%1px").arg(itemGeometry.leftPadding / zoom),
                                           Qt::AlignTop | Qt::AlignHCenter);
    }

    if (!qIsNaN(itemGeometry.rightPadding)) {
        drawHorizontalAnchor(itemGeometry,
                             itemGeometry.itemRect.right(),
                             itemGeometry.rightPadding);
        texts << drawHorizontalAnchorLabel(itemGeometry,
                                           itemGeometry.itemRect.right(),
                                           itemGeometry.rightPadding,
                                           QStringLiteral("%1px").arg(itemGeometry.rightPadding / zoom),
                                           Qt::AlignBottom | Qt::AlignHCenter);
    }

    if (!qIsNaN(itemGeometry.topPadding)) {
        drawVerticalAnchor(itemGeometry,
                           itemGeometry.itemRect.top(),
                           -itemGeometry.topPadding);
        texts << drawVerticalAnchorLabel(itemGeometry,
                                         itemGeometry.itemRect.top(),
                                         -itemGeometry.topPadding,
                                         QStringLiteral("%1px").arg(itemGeometry.topPadding / zoom),
                                         Qt::AlignVCenter | Qt::AlignLeft);
    }

    if (!qIsNaN(itemGeometry.bottomPadding)) {
        drawVerticalAnchor(itemGeometry,
                           itemGeometry.itemRect.bottom(),
                           itemGeometry.bottomPadding);
        texts << drawVerticalAnchorLabel(itemGeometry,
                                         itemGeometry.itemRect.bottom(),
                                         itemGeometry.bottomPadding,
                                         QStringLiteral("%1px").arg(itemGeometry.bottomPadding / zoom),
                                         Qt::AlignVCenter | Qt::AlignRight);
    }

    // Finally draw texts over the traced rectangles and lines
    // This make sure texts are always readable
    for (const auto &t : qAsConst(texts)) {
        if (t.label.isEmpty())
            continue;

        m_painter->setPen(t.pen);
        m_painter->drawText(t.rect, t.align, t.label);
    }

    m_painter->restore();
}

void QuickDecorationsDrawer::drawTraces()
{
    const QVector<QuickItemGeometry> itemsGeometry(this->itemsGeometry());

    if (itemsGeometry.isEmpty())
        return;

    m_painter->save();

    for (auto itemGeometry : itemsGeometry) {
        itemGeometry.scaleTo(m_renderInfo->zoom);

        // bounding box
        m_painter->setPen(itemGeometry.traceColor);
        m_painter->setBrush(itemGeometry.traceColor.lighter());
        m_painter->drawRect(itemGeometry.boundingRect);

        // bounding class box
        m_painter->setPen(QColor(60, 60, 60, 70));
        m_painter->setBrush(m_painter->pen().color());

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
        const int margin = m_painter->fontMetrics().width(QLatin1Char('X')) / 2;
#else
        const int margin = m_painter->fontMetrics().horizontalAdvance(QLatin1Char('X')) / 2;
#endif
        const QRectF classRect =
            itemGeometry.boundingRect.adjusted(
                0, 0, 0,
                -(itemGeometry.boundingRect.height() - (m_painter->fontMetrics().height() * 1.6)));
        m_painter->drawRect(classRect);

        // type name label
        m_painter->setPen(QColor(250, 250, 250, 120));
        m_painter->drawText(classRect.adjusted(margin, margin, -margin, -margin),
                          Qt::AlignVCenter | Qt::AlignLeft | Qt::TextDontClip,
                          itemGeometry.traceTypeName);

        // draw bounding box corners
        m_painter->setPen(QPen(QColor(30, 30, 30), 3));
        m_painter->setBrush(m_painter->pen().color());
        const QVector<QPointF> points = QVector<QPointF>()
                << itemGeometry.boundingRect.topLeft()
                << itemGeometry.boundingRect.topRight()
                << itemGeometry.boundingRect.bottomRight()
                << itemGeometry.boundingRect.bottomLeft();
        m_painter->drawPoints(points);

        // name label
        m_painter->setPen(QColor(250, 250, 250, 120));
        m_painter->drawText(itemGeometry.boundingRect.adjusted(margin, margin, -margin, -margin),
                          Qt::AlignVCenter | Qt::AlignLeft | Qt::TextDontClip,
                          itemGeometry.traceName);
    }

    m_painter->restore();
}

QuickItemGeometry QuickDecorationsDrawer::itemGeometry() const
{
    switch (m_type) {
    case QuickDecorationsDrawer::Decorations:
        return static_cast<const QuickDecorationsRenderInfo *>(m_renderInfo)->itemGeometry;
    case QuickDecorationsDrawer::Traces:
        break;
    }

    return QuickItemGeometry();
}

QVector<QuickItemGeometry> QuickDecorationsDrawer::itemsGeometry() const
{
    switch (m_type) {
    case QuickDecorationsDrawer::Decorations:
        break;
    case QuickDecorationsDrawer::Traces:
        return static_cast<const QuickDecorationsTracesInfo *>(m_renderInfo)->itemsGeometry;
    }

    return QVector<QuickItemGeometry>();
}

void QuickDecorationsDrawer::drawGrid()
{
    const QRectF &viewRect(m_renderInfo->viewRect);
    const QPointF &gridOffset(m_renderInfo->settings.gridOffset);
    const QSizeF &gridCellSize(m_renderInfo->settings.gridCellSize);

    if (!m_renderInfo->settings.gridEnabled || gridCellSize.isEmpty())
        return;

    m_painter->save();
    m_painter->setPen(m_renderInfo->settings.gridColor);

    QVector<QLineF> lines;
    lines.reserve((viewRect.width() / gridCellSize.width()) +
                  (viewRect.height() / gridCellSize.height()));

    for (qreal x = viewRect.left() + gridOffset.x(); x < viewRect.right(); x += gridCellSize.width()) {
        if (x < viewRect.left())
            continue;

        lines << QLineF(QPointF(x, viewRect.top()) * m_renderInfo->zoom,
                        QPointF(x, viewRect.bottom()) * m_renderInfo->zoom);
    }

    for (qreal y = viewRect.top() + gridOffset.y(); y < viewRect.bottom(); y += gridCellSize.height()) {
        if (y < viewRect.top())
            continue;

        lines << QLineF(QPointF(viewRect.left(), y) * m_renderInfo->zoom,
                        QPointF(viewRect.right(), y) * m_renderInfo->zoom);
    }

    m_painter->drawLines(lines);
    m_painter->restore();
}

void QuickDecorationsDrawer::drawArrow(const QPointF &first, const QPointF &second)
{
    m_painter->drawLine(first, second);
    QPointF vector(second - first);
    QTransform m;
    m.rotate(30);
    QVector2D v1 = QVector2D(m.map(vector)).normalized() * 10;
    m.rotate(-60);
    QVector2D v2 = QVector2D(m.map(vector)).normalized() * 10;
    m_painter->drawLine(first, first + v1.toPointF());
    m_painter->drawLine(first, first + v2.toPointF());
    m_painter->drawLine(second, second - v1.toPointF());
    m_painter->drawLine(second, second - v2.toPointF());
}

void QuickDecorationsDrawer::drawAnchor(const QuickItemGeometry &itemGeometry, Qt::Orientation orientation, qreal ownAnchorLine, qreal offset)
{
    const QRectF &viewRect(m_renderInfo->viewRect);
    const qreal &zoom(m_renderInfo->zoom);

    m_painter->save();

    const qreal foreignAnchorLine = ownAnchorLine - offset;

    // Arrow
    if (offset) {
        if (orientation == Qt::Horizontal) {
            drawArrow(QPointF(foreignAnchorLine,
                              (itemGeometry.itemRect.top() + itemGeometry.itemRect.bottom()) / 2),
                      QPointF(ownAnchorLine,
                              (itemGeometry.itemRect.top() + itemGeometry.itemRect.bottom()) / 2));
        } else {
            drawArrow(QPointF((itemGeometry.itemRect.left() + itemGeometry.itemRect.right()) / 2,
                              foreignAnchorLine),
                      QPointF((itemGeometry.itemRect.left() + itemGeometry.itemRect.right()) / 2,
                              ownAnchorLine));
        }
    }

    // Own Anchor line
    QPen pen(m_painter->pen());
    pen.setWidth(2);
    m_painter->setPen(pen);
    if (orientation == Qt::Horizontal) {
        m_painter->drawLine(ownAnchorLine,
                    itemGeometry.itemRect.top(), ownAnchorLine,
                    itemGeometry.itemRect.bottom());
    } else {
        m_painter->drawLine(
                    itemGeometry.itemRect.left(), ownAnchorLine,
                    itemGeometry.itemRect.right(), ownAnchorLine);
    }

    // Foreign Anchor line
    pen.setStyle(Qt::DotLine);
    m_painter->setPen(pen);
    if (orientation == Qt::Horizontal) {
        m_painter->drawLine(foreignAnchorLine, 0, foreignAnchorLine, viewRect.height() * zoom);
    } else {
        m_painter->drawLine(0, foreignAnchorLine, viewRect.width() * zoom, foreignAnchorLine);
    }

    m_painter->restore();
}

void QuickDecorationsDrawer::drawVerticalAnchor(const QuickItemGeometry &itemGeometry,
                                                qreal ownAnchorLine, qreal offset)
{
    drawAnchor(itemGeometry, Qt::Vertical, ownAnchorLine, offset);
}

void QuickDecorationsDrawer::drawHorizontalAnchor(const QuickItemGeometry &itemGeometry,
                                                  qreal ownAnchorLine, qreal offset)
{
    drawAnchor(itemGeometry, Qt::Horizontal, ownAnchorLine, offset);
}

QuickDecorationsDrawer::DrawTextInfo QuickDecorationsDrawer::drawAnchorLabel(const QuickItemGeometry &itemGeometry,
                                                         Qt::Orientation orientation, qreal ownAnchorLine, qreal offset, const QString &label, Qt::Alignment align)
{
    if (align.testFlag(Qt::AlignCenter)) {
        qWarning("%s: You can not use Qt::AlignCenter!", Q_FUNC_INFO);
        return QuickDecorationsDrawer::DrawTextInfo();
    }

    if (align.testFlag(Qt::AlignJustify)) {
        qWarning("%s: You can not use Qt::AlignJustify!", Q_FUNC_INFO);
        return QuickDecorationsDrawer::DrawTextInfo();
    }

    if (align.testFlag(Qt::AlignBaseline)) {
        qWarning("%s: You can not use Qt::AlignBaseline!", Q_FUNC_INFO);
        return QuickDecorationsDrawer::DrawTextInfo();
    }

    if (offset) {
        const qreal foreignAnchorLine = ownAnchorLine - offset;
        const int margin = 10;

        if (orientation == Qt::Horizontal) {
            QRectF rect(m_painter->fontMetrics().boundingRect(label));

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

            return QuickDecorationsDrawer::DrawTextInfo(
                        m_painter->pen(),
                        rect,
                        label
                        );
        } else {
            QRectF rect(m_painter->fontMetrics().boundingRect(label));

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

            return QuickDecorationsDrawer::DrawTextInfo(
                        m_painter->pen(),
                        rect,
                        label
                        );
        }
    }

    return QuickDecorationsDrawer::DrawTextInfo();
}

QuickDecorationsDrawer::DrawTextInfo QuickDecorationsDrawer::drawHorizontalAnchorLabel(const QuickItemGeometry &itemGeometry,
                                                                   qreal ownAnchorLine, qreal offset, const QString &label, Qt::Alignment align)
{
    return drawAnchorLabel(itemGeometry, Qt::Horizontal, ownAnchorLine, offset, label, align);
}

QuickDecorationsDrawer::DrawTextInfo QuickDecorationsDrawer::drawVerticalAnchorLabel(const QuickItemGeometry &itemGeometry,
                                                                 qreal ownAnchorLine, qreal offset, const QString &label, Qt::Alignment align)
{
    return drawAnchorLabel(itemGeometry, Qt::Vertical, ownAnchorLine, offset, label, align);
}

QDataStream &GammaRay::operator<<(QDataStream &stream, const GammaRay::QuickDecorationsSettings &settings)
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
            << settings.gridOffset
            << settings.gridCellSize
            << settings.gridColor
            << settings.componentsTraces
            << settings.gridEnabled
    ;

    return stream;
}

QDataStream &GammaRay::operator>>(QDataStream &stream, GammaRay::QuickDecorationsSettings &settings)
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
            >> settings.gridOffset
            >> settings.gridCellSize
            >> settings.gridColor
            >> settings.componentsTraces
            >> settings.gridEnabled
    ;

    return stream;
}
