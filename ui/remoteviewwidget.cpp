/*
  remoteviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "remoteviewwidget.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>

using namespace GammaRay;

RemoteViewWidget::RemoteViewWidget(QWidget* parent):
    QWidget(parent),
    m_zoom(1.0),
    m_x(0),
    m_y(0),
    m_interactionMode(ViewInteraction),
    m_mouseDown(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    setMinimumSize(QSize(400, 300));
    setFocusPolicy(Qt::StrongFocus);

    // Background
    QPixmap bgPattern(20, 20);
    bgPattern.fill(Qt::lightGray);
    QPainter bgPainter(&bgPattern);
    bgPainter.fillRect(10, 0, 10, 10, Qt::gray);
    bgPainter.fillRect(0, 10, 10, 10, Qt::gray);
    m_backgroundBrush.setTexture(bgPattern);

    m_zoomLevels.reserve(8);
    m_zoomLevels <<  .125 << .25 << .5 << 1.0 << 2.0 << 4.0 << 8.0 << 16.0;
}

void RemoteViewWidget::setImage(const QImage& image)
{
    if (m_sourceImage.isNull()) {
        m_x = 0.5 * (contentWidth() - image.width() * m_zoom);
        m_y = 0.5 * (contentHeight() - image.height() * m_zoom);
    }

    m_sourceImage = image;
    update();
}

void RemoteViewWidget::setZoom(double zoom)
{
    Q_ASSERT(!m_zoomLevels.isEmpty());
    const auto oldZoom = m_zoom;

    // snap to nearest zoom level
    auto it = std::lower_bound(m_zoomLevels.constBegin(), m_zoomLevels.constEnd(), zoom);
    if (it == m_zoomLevels.constEnd()) {
        m_zoom = m_zoomLevels.last();
    } else if (it == m_zoomLevels.constBegin()) {
        m_zoom = *it;
    } else {
        auto delta = (*it) - zoom;
        m_zoom = *it;
        --it;
        if (zoom - (*it) < delta)
            m_zoom = *it;
    }

    if (oldZoom == m_zoom)
        return;
    // TODO: emit changed signal, for updating external UI

    m_x = contentWidth() / 2 - (contentWidth() / 2 - m_x) * zoom / oldZoom;
    m_y = contentHeight() / 2 - (contentHeight() / 2 - m_y) * zoom / oldZoom;
    update();
}

void RemoteViewWidget::zoomIn()
{
    Q_ASSERT(!m_zoomLevels.isEmpty());
    auto it = std::lower_bound(m_zoomLevels.constBegin(), m_zoomLevels.constEnd(), m_zoom);
    if (it == m_zoomLevels.constEnd())
        return;
    ++it;
    if (it == m_zoomLevels.constEnd())
        return;
    setZoom(*it);
}

void RemoteViewWidget::zoomOut()
{
    Q_ASSERT(!m_zoomLevels.isEmpty());
    auto it = std::lower_bound(m_zoomLevels.constBegin(), m_zoomLevels.constEnd(), m_zoom);
    if (it == m_zoomLevels.constBegin())
        return;
    --it;
    setZoom(*it);
}

void RemoteViewWidget::fitToView()
{
    const auto scale = std::min<double>(1.0, std::min((double)contentWidth() / (double)m_sourceImage.width(), (double)contentHeight() / (double)m_sourceImage.height()));
    setZoom(scale);
    m_x = 0.5 * (contentWidth() - m_sourceImage.width() * m_zoom);
    m_y = 0.5 * (contentHeight() - m_sourceImage.height() * m_zoom);
    update();
}

void RemoteViewWidget::setInteractionMode(RemoteViewWidget::InteractionMode mode)
{
    m_interactionMode = mode;
}

void RemoteViewWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.fillRect(rect(), m_backgroundBrush);

    if (m_sourceImage.isNull()) {
        p.drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, tr("No Preview available.")); // TODO customizable text by tool
        return;
    }

    p.save();
    p.setTransform(QTransform::fromTranslate(m_x, m_y));
    if (m_zoom < 1.0) { // We want the preview to look nice when zoomed out,
                        // but need to be able to see single pixels when zoomed in.
        p.setRenderHint(QPainter::SmoothPixmapTransform);
    }
    p.drawImage(QRect(QPoint(0, 0), m_sourceImage.size() * m_zoom), m_sourceImage);
    p.restore();

    drawRuler(&p);

    if (m_interactionMode == Measuring && m_mouseDown) {
        drawMeasureOverlay(&p);
    }
}

static int tickLength(int pos, int lineLength, qreal zoom)
{
    if (pos % 100 == 0) // the one getting a label
      return lineLength * 2;
    if ((int)(pos / zoom) % 10 == 0)
      return lineLength + 2;
    if ((int)(pos / zoom) % 5 == 0)
      return lineLength + 1;
    return lineLength;
}

void RemoteViewWidget::drawRuler(QPainter* p)
{
    p->save();

    const int hRulerHeight = horizontalRulerHeight();
    const int vRulerWidth = verticalRulerWidth();
    const int lineLength = 8;
    const int pixelSteps = std::max<int>(2, m_zoom);

    p->setPen(Qt::NoPen);
    p->setBrush(QBrush(QColor(51, 51, 51, 170)));
    p->drawRect(QRect(0, height() - hRulerHeight, width(), hRulerHeight));
    p->drawRect(QRect(width() - vRulerWidth, 0, vRulerWidth, height() - hRulerHeight));

    const auto activePen = QPen(QColor(255, 255, 255, 170));
    const auto inactivePen = QPen(QColor(0, 0, 0, 170));

    // horizontal ruler at the bottom
    if (m_x <= 0) {
      p->setPen(activePen);
    } else {
      p->setPen(inactivePen);
    }
    for (int x = -m_x; x < width() - m_x - vRulerWidth; ++x) {
        if (x == 0) {
            p->setPen(activePen);
        }
        if (x % pixelSteps == 0) {
            p->drawLine(m_x + x, height() - hRulerHeight, m_x + x, height() - hRulerHeight + tickLength(x, lineLength, m_zoom));
        }

        if (x % 100 == 0 && x >= 0 && x <= m_sourceImage.width() * m_zoom) {
            auto nearestTick = qRound(x / m_zoom * 0.2) * 5;
            auto xOffset = (nearestTick - (x/m_zoom)) * m_zoom;
            p->drawText(m_x + x + xOffset - 20, height() - (hRulerHeight - 2 * lineLength),
                        40, hRulerHeight - 2 * lineLength,
                        Qt::AlignHCenter | Qt::AlignVCenter, QString::number((int)nearestTick));
        }


        if (m_sourceImage.width() * m_zoom - x < 1) {
            p->setPen(inactivePen);
        }
    }

    // vertical ruler on the right
    if (m_y <= 0) {
      p->setPen(activePen);
    } else {
      p->setPen(inactivePen);
    }
    for (int y = -m_y; y < height() - m_y - hRulerHeight; ++y) {
        if (y == 0) {
            p->setPen(activePen);
        }
        if (y % pixelSteps == 0) {
            p->drawLine(width() - vRulerWidth, m_y + y, width() - vRulerWidth + tickLength(y, lineLength, m_zoom), m_y + y);
        }

        if (y % 100 == 0 && y >= 0 && y <= m_sourceImage.height() * m_zoom) {
            auto nearestTick = qRound(y / m_zoom * 0.2) * 5;
            auto yOffset = (nearestTick - (y/m_zoom)) * m_zoom;
            p->drawText(width() - (vRulerWidth - 2 * lineLength), m_y + y + yOffset - 20,
                        vRulerWidth - 2 * lineLength, 40,
                        Qt::AlignHCenter | Qt::AlignVCenter, QString::number((int)nearestTick));
        }


        if (m_sourceImage.height() * m_zoom - y < 1) {
            p->setPen(inactivePen);
        }
    }

    p->setPen(activePen);
    p->drawText(QRect(width() - vRulerWidth, height() - hRulerHeight, vRulerWidth, hRulerHeight),
                QStringLiteral("%1x\n%2").arg(m_currentMousePosition.x()).arg(m_currentMousePosition.y()),
                Qt::AlignHCenter | Qt::AlignVCenter
               );
    p->restore();
}

void RemoteViewWidget::drawMeasureOverlay(QPainter* p)
{
    // TODO
}

QPoint RemoteViewWidget::mapToSource(QPoint pos)
{
    return (pos - QPoint(m_x, m_y)) / m_zoom;
}

void RemoteViewWidget::resizeEvent(QResizeEvent* event)
{
    m_x += 0.5 * (event->size().width() - event->oldSize().width());
    m_y += 0.5 * (event->size().height() - event->oldSize().height());

    QWidget::resizeEvent(event);
}

void RemoteViewWidget::mousePressEvent(QMouseEvent* event)
{
    m_mouseDown = true;
    m_currentMousePosition = mapToSource(event->pos());

    switch (m_interactionMode) {
        case ViewInteraction:
            m_mouseDownPosition = event->pos() - QPoint(m_x, m_y);
            // if (e->modifiers() Qt::ControlModifier)
                // pickElement(mapToSource(event->pos());
             break;
        case Measuring:
            m_mouseDownPosition = mapToSource(event->pos());
            break;
        case ElementPicking:
            // pickElement(mapToSource(event->pos()));
            break;
        case InputRedirection:
            //TODO
            break;
    }

    QWidget::mousePressEvent(event);
}

void RemoteViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_mouseDown = false;
    m_currentMousePosition = mapToSource(event->pos());

    switch (m_interactionMode) {
        case InputRedirection:
            // TODO
            break;
    }

    QWidget::mouseReleaseEvent(event);
}

void RemoteViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_currentMousePosition = mapToSource(event->pos());

    switch (m_interactionMode) {
        case ViewInteraction:
            if (event->buttons() != Qt::LeftButton)
                break;
            m_x = event->x() - m_mouseDownPosition.x();
            m_y = event->y() - m_mouseDownPosition.y();

            if (m_x > width() / 2) {
                m_x = width() / 2;
            } else if (m_x + m_sourceImage.width() * m_zoom < width() / 2.0) {
                m_x = width() / 2 - m_sourceImage.width() * m_zoom;
            }
            if (m_y > height() / 2) {
                m_y = height() / 2;
            } else if (m_y + m_sourceImage.height() * m_zoom < height() / 2.0) {
                m_y = height() / 2 - m_sourceImage.height() * m_zoom;
            }
            break;
        case InputRedirection:
            // TODO
            break;
    }

  update();
}

void RemoteViewWidget::wheelEvent(QWheelEvent *event)
{
    switch (m_interactionMode) {
        case ViewInteraction:
        case ElementPicking:
            // TODO pan if Ctrl isn't pressed
            if (event->modifiers() & Qt::ControlModifier && event->orientation() == Qt::Vertical) {
                if (event->delta() > 0)
                    zoomIn();
                else
                    zoomOut();
            }
            break;
        case InputRedirection:
            // TODO
            break;
    }

    QWidget::wheelEvent(event);
}

void RemoteViewWidget::keyPressEvent(QKeyEvent* event)
{
    switch (m_interactionMode) {
        case ViewInteraction:
        case ElementPicking:
            if (event->key() == Qt::Key_Plus && event->modifiers() & Qt::ControlModifier)
                zoomIn();
            if (event->key() == Qt::Key_Minus && event->modifiers() & Qt::ControlModifier)
                zoomOut();
            break;
        case InputRedirection:
            // TODO
            break;
    }
    QWidget::keyPressEvent(event);
}

void RemoteViewWidget::keyReleaseEvent(QKeyEvent* event)
{
    switch (m_interactionMode) {
        case InputRedirection:
            // TODO
            break;
    }
    QWidget::keyReleaseEvent(event);
}

int RemoteViewWidget::contentWidth() const
{
    return width() - verticalRulerWidth();
}

int RemoteViewWidget::contentHeight() const
{
    return height() - horizontalRulerHeight();
}

int RemoteViewWidget::verticalRulerWidth() const
{
    return 50; // TODO depends on font and longest tick label
}

int RemoteViewWidget::horizontalRulerHeight() const
{
    return 35; // TODO depends on font size
}
