/*
  quickscenepreviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "quickscenepreviewwidget.h"
#include "quickinspectorinterface.h"

#include <QPaintEvent>
#include <QPainter>
#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QToolBar>

#include <cmath>

using namespace GammaRay;

QuickScenePreviewWidget::QuickScenePreviewWidget(QuickInspectorInterface *inspector, QWidget *parent)
  : QWidget(parent),
    m_inspectorInterface(inspector),
    m_mouseMode(MovePreview),
    m_zoom(1),
    m_x(0),
    m_y(0),
    m_mousePressed(false)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setMouseTracking(true);
  setAttribute(Qt::WA_OpaquePaintEvent);
  setMinimumSize(QSize(400, 300));


  // Background
  QPixmap bgPattern(20, 20);
  bgPattern.fill(Qt::lightGray);
  QPainter bgPainter(&bgPattern);
  bgPainter.fillRect(10, 0, 10, 10, Qt::gray);
  bgPainter.fillRect(0, 10, 10, 10, Qt::gray);
  m_backgroundBrush.setTexture(bgPattern);


  // Toolbar
  m_toolBar.toolbarWidget = new QToolBar(this);
  m_toolBar.toolbarWidget->setAutoFillBackground(true);

  m_toolBar.visualizeGroup = new QActionGroup(this);
  m_toolBar.visualizeGroup->setExclusive(false); // we need 0 or 1 selected, not exactly 1
  m_toolBar.visualizeClipping = new QAction(QIcon(QStringLiteral(":/gammaray/plugins/quickinspector/visualize-clipping.png")), tr("Visualize Clipping"), this);
  m_toolBar.visualizeClipping->setActionGroup(m_toolBar.visualizeGroup);
  m_toolBar.visualizeClipping->setCheckable(true);
  m_toolBar.visualizeClipping->setToolTip(tr("<b>Visualize Clipping</b><br/>"
      "Items with the property <i>clip</i> set to true, will cut off their and their "
      "children's rendering at the items' bounds. While this is a handy feature it "
      "comes with quite some cost, like disabling some performance optimizations.<br/>"
      "With this tool enabled the QtQuick renderer highlights items, that have clipping "
      "enabled, so you can check for items, that have clipping enabled unnecessarily. "));
  m_toolBar.toolbarWidget->addAction(m_toolBar.visualizeClipping);
  connect(m_toolBar.visualizeClipping, SIGNAL(triggered(bool)), this, SLOT(visualizeActionTriggered(bool)));

  m_toolBar.visualizeOverdraw = new QAction(QIcon(QStringLiteral(":/gammaray/plugins/quickinspector/visualize-overdraw.png")), tr("Visualize Overdraw"), this);
  m_toolBar.visualizeOverdraw->setActionGroup(m_toolBar.visualizeGroup);
  m_toolBar.visualizeOverdraw->setCheckable(true);
  m_toolBar.visualizeOverdraw->setToolTip(tr("<b>Visualize Overdraw</b><br/>"
      "The QtQuick renderer doesn't detect if an item is obscured by another "
      "opaque item, is completely outside the scene or outside a clipped ancestor and "
      "thus doesn't need to be rendered. You thus need to take care of setting "
      "<i>visible: false</i> for hidden items, yourself.<br/>"
      "With this tool enabled the QtQuick renderer draws a 3D-Box visualizing the "
      "layers of items that are drawn."));
  m_toolBar.toolbarWidget->addAction(m_toolBar.visualizeOverdraw);
  connect(m_toolBar.visualizeOverdraw, SIGNAL(triggered(bool)), this, SLOT(visualizeActionTriggered(bool)));

  m_toolBar.visualizeBatches = new QAction(QIcon(QStringLiteral(":/gammaray/plugins/quickinspector/visualize-batches.png")), tr("Visualize Batches"), this);
  m_toolBar.visualizeBatches->setActionGroup(m_toolBar.visualizeGroup);
  m_toolBar.visualizeBatches->setCheckable(true);
  m_toolBar.visualizeBatches->setToolTip(tr("<b>Visualize Batches</b><br/>"
      "Where a traditional 2D API, such as QPainter, Cairo or Context2D, is written to "
      "handle thousands of individual draw calls per frame, OpenGL is a pure hardware "
      "API and performs best when the number of draw calls is very low and state "
      "changes are kept to a minimum. Therefore the QtQuick renderer combines the "
      "rendering of similar items into single batches.<br/>"
      "Some settings (like <i>clip: true</i>) will cause the batching to fail, though, "
      "causing items to be rendered separately. With this tool enabled the QtQuick "
      "renderer visualizes those batches, by drawing all items that are batched using "
      "the same color. The fewer colors you see in this mode the better."));
  m_toolBar.toolbarWidget->addAction(m_toolBar.visualizeBatches);
  connect(m_toolBar.visualizeBatches, SIGNAL(triggered(bool)), this, SLOT(visualizeActionTriggered(bool)));

  m_toolBar.visualizeChanges = new QAction(QIcon(QStringLiteral(":/gammaray/plugins/quickinspector/visualize-changes.png")), tr("Visualize Changes"), this);
  m_toolBar.visualizeChanges->setActionGroup(m_toolBar.visualizeGroup);
  m_toolBar.visualizeChanges->setCheckable(true);
  m_toolBar.visualizeChanges->setToolTip(tr("<b>Visualize Changes</b><br>"
      "The QtQuick scene is only repainted, if some item changes in a visual manner. "
      "Unnecessary repaints can have a bad impact on the performance. With this tool "
      "enabled, the QtQuick renderer will thus on each repaint highlight the item(s), "
      "that caused the repaint."));
  m_toolBar.toolbarWidget->addAction(m_toolBar.visualizeChanges);
  connect(m_toolBar.visualizeChanges, SIGNAL(triggered(bool)), this, SLOT(visualizeActionTriggered(bool)));

  m_toolBar.toolbarWidget->addSeparator();


  QActionGroup *mouseToolGroup = new QActionGroup(this);
  mouseToolGroup->setExclusive(true);
  connect(mouseToolGroup, SIGNAL(triggered(QAction*)), this, SLOT(setMouseTool(QAction*)));

  m_toolBar.movePreview = new QAction(QIcon(QStringLiteral(":/gammaray/plugins/quickinspector/move-preview.png")), tr("Move Preview"), this);
  m_toolBar.movePreview->setActionGroup(mouseToolGroup);
  m_toolBar.movePreview->setToolTip(tr("<b>Move preview</b><br>"
      "Default mode. Click and drag to move the preview. Won't impact the original "
      "application in any way. "));
  m_toolBar.movePreview->setCheckable(true);
  m_toolBar.movePreview->setChecked(true);
  m_toolBar.toolbarWidget->addAction(m_toolBar.movePreview);

  m_toolBar.measurePixels = new QAction(QIcon(QStringLiteral(":/gammaray/plugins/quickinspector/measure-pixels.png")), tr("Measure Pizel Sizes"), this);
  m_toolBar.measurePixels->setActionGroup(mouseToolGroup);
  m_toolBar.measurePixels->setToolTip(tr("<b>Measure pixel-sizes</b><br>"
      "Choose this mode, click somewhere and drag to measure the distance between the "
      "point you clicked and the point where your mouse pointer is. (Measured in scene "
      "coordinates)."));
  m_toolBar.measurePixels->setCheckable(true);
  m_toolBar.toolbarWidget->addAction(m_toolBar.measurePixels);

  m_toolBar.redirectInput = new QAction(QIcon(QStringLiteral(":/gammaray/plugins/quickinspector/redirect-input.png")), tr("Redirect Input"), this);
  m_toolBar.redirectInput->setActionGroup(mouseToolGroup);
  m_toolBar.redirectInput->setToolTip(tr("<b>Redirect Input</b><br>"
      "In this mode all mouse input is redirected directly to the original application,"
      "so you can control the application directly from within GammaRay."));
  m_toolBar.redirectInput->setCheckable(true);
  m_toolBar.toolbarWidget->addAction(m_toolBar.redirectInput);

  m_toolBar.toolbarWidget->addSeparator();


  m_toolBar.zoomCombobox = new QComboBox(this);
  m_toolBar.zoomCombobox->addItem(tr("12.5 %"), .125);
  m_toolBar.zoomCombobox->addItem(tr("20 %"), .2);
  m_toolBar.zoomCombobox->addItem(tr("25 %"), .25);
  m_toolBar.zoomCombobox->addItem(tr("33 %"), 1.0/3);
  m_toolBar.zoomCombobox->addItem(tr("50 %"), .5);
  m_toolBar.zoomCombobox->addItem(tr("100 %"), 1);
  m_toolBar.zoomCombobox->setCurrentIndex(m_toolBar.zoomCombobox->count() - 1);
  m_toolBar.zoomCombobox->addItem(tr("200 %"), 2);
  m_toolBar.zoomCombobox->addItem(tr("300 %"), 3);
  m_toolBar.zoomCombobox->addItem(tr("400 %"), 4);
  m_toolBar.zoomCombobox->addItem(tr("500 %"), 5);
  m_toolBar.zoomCombobox->addItem(tr("1000 %"), 10);
  m_toolBar.zoomCombobox->addItem(tr("2000 %"), 20);
  connect(m_toolBar.zoomCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(setZoomFromCombobox(int)));

  m_toolBar.toolbarWidget->addWidget(m_toolBar.zoomCombobox);

  m_toolBar.measureLabel = new QLabel(this);
  m_toolBar.toolbarWidget->addWidget(m_toolBar.measureLabel);
}

QuickScenePreviewWidget::~QuickScenePreviewWidget()
{
}

void QuickScenePreviewWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), m_backgroundBrush);

  drawGeometry(&p);

  drawRuler(&p);

  if (m_mouseMode == MeasurePixels && m_mousePressed) {
    drawMeasureLine(&p);
  }
}

void QuickScenePreviewWidget::resizeEvent(QResizeEvent *e)
{
    m_toolBar.toolbarWidget->setGeometry(0, 0, width(), m_toolBar.toolbarWidget->sizeHint().height());

    m_x += 0.5 * (e->size().width() - e->oldSize().width());
    m_y += 0.5 * (e->size().height() - e->oldSize().height());
}

void QuickScenePreviewWidget::drawGeometry(QPainter* p)
{
  p->save();

  if (m_image.isNull()) {
    p->drawText(rect(), Qt::AlignHCenter | Qt::AlignVCenter, tr("No Preview available.\n"
                "(This happens e.g. when the window is minimized or the scene is hidden)"));
    p->restore();
    return;
  }

  p->setTransform(QTransform::fromTranslate(m_x, m_y));
  p->fillRect(QRect(QPoint(0, 0), m_image.size() * m_zoom), Qt::white);
  if (m_zoom < 1) { // We want the preview to look nice when zoomed out,
                    // but need to be able to see single pixels when zoomed in.
    p->setRenderHint(QPainter::SmoothPixmapTransform);
  }
  p->drawImage(QRect(QPoint(0, 0), m_image.size() * m_zoom), m_image);

  // bounding box
  p->setPen(QColor(232, 87, 82, 170));
  p->setBrush(QBrush(QColor(232, 87, 82, 95)));
  p->drawRect(m_effectiveGeometry.boundingRect);

  // original geometry
  if (m_effectiveGeometry.itemRect != m_effectiveGeometry.boundingRect) {
    p->setPen(Qt::gray);
    p->setBrush(QBrush(Qt::gray, Qt::BDiagPattern));
    p->drawRect(m_effectiveGeometry.itemRect);
  }

  // children rect
  if (m_effectiveGeometry.itemRect != m_effectiveGeometry.boundingRect && m_effectiveGeometry.transform.isIdentity()) {
    // If this item is transformed the children rect will be painted wrongly,
    // so for now skip painting it.
    p->setPen(QColor(0, 99, 193, 170));
    p->setBrush(QBrush(QColor(0, 99, 193, 95)));
    p->drawRect(m_effectiveGeometry.childrenRect);
  }

  // transform origin
  if (m_effectiveGeometry.itemRect != m_effectiveGeometry.boundingRect) {
    p->setPen(QColor(156, 15, 86, 170));
    p->drawEllipse(m_effectiveGeometry.transformOriginPoint, 2.5, 2.5);
    p->drawLine(m_effectiveGeometry.transformOriginPoint - QPointF(0, 6), m_effectiveGeometry.transformOriginPoint + QPointF(0, 6));
    p->drawLine(m_effectiveGeometry.transformOriginPoint - QPointF(6, 0), m_effectiveGeometry.transformOriginPoint + QPointF(6, 0));
  }

  // x and y values
  p->setPen(QColor(136, 136, 136));
  if (!m_effectiveGeometry.left &&
      !m_effectiveGeometry.horizontalCenter &&
      !m_effectiveGeometry.right &&
      m_effectiveGeometry.x != 0) {
    QPointF parentEnd = (QPointF(m_effectiveGeometry.itemRect.x() - m_effectiveGeometry.x, m_effectiveGeometry.itemRect.y()));
    QPointF itemEnd = m_effectiveGeometry.itemRect.topLeft();
    drawArrow(p, parentEnd, itemEnd);
    p->drawText(QRectF(parentEnd.x(), parentEnd.y() + 10, itemEnd.x() - parentEnd.x(), 50),
                Qt::AlignHCenter | Qt::TextDontClip,
                QStringLiteral("x: %1px").arg(m_effectiveGeometry.x / m_zoom));
  }
  if (!m_effectiveGeometry.top &&
      !m_effectiveGeometry.verticalCenter &&
      !m_effectiveGeometry.bottom &&
      !m_effectiveGeometry.baseline &&
      m_effectiveGeometry.y != 0) {
    QPointF parentEnd = (QPointF(m_effectiveGeometry.itemRect.x(), m_effectiveGeometry.itemRect.y() - m_effectiveGeometry.y));
    QPointF itemEnd = m_effectiveGeometry.itemRect.topLeft();
    drawArrow(p, parentEnd, itemEnd);
    p->drawText(QRectF(parentEnd.x() + 10, parentEnd.y(), 100, itemEnd.y() - parentEnd.y()),
                Qt::AlignVCenter | Qt::TextDontClip,
                QStringLiteral("y: %1px").arg(m_effectiveGeometry.y / m_zoom));
  }

  // anchors
  if (m_effectiveGeometry.left) {
    drawAnchor(p, Qt::Horizontal,
               m_effectiveGeometry.itemRect.left(), m_effectiveGeometry.leftMargin,
               QStringLiteral("margin: %1px").arg(m_effectiveGeometry.leftMargin / m_zoom));
  }

  if (m_effectiveGeometry.horizontalCenter) {
    drawAnchor(p, Qt::Horizontal,
               (m_effectiveGeometry.itemRect.left() + m_effectiveGeometry.itemRect.right()) / 2, m_effectiveGeometry.horizontalCenterOffset,
               QStringLiteral("offset: %1px").arg(m_effectiveGeometry.horizontalCenterOffset / m_zoom));
  }

  if (m_effectiveGeometry.right) {
    drawAnchor(p, Qt::Horizontal,
               m_effectiveGeometry.itemRect.right(), -m_effectiveGeometry.rightMargin,
               QStringLiteral("margin: %1px").arg(m_effectiveGeometry.rightMargin / m_zoom));
  }

  if (m_effectiveGeometry.top) {
    drawAnchor(p, Qt::Vertical,
               m_effectiveGeometry.itemRect.top(), m_effectiveGeometry.topMargin,
               QStringLiteral("margin: %1px").arg(m_effectiveGeometry.topMargin / m_zoom));
  }

  if (m_effectiveGeometry.verticalCenter) {
    drawAnchor(p, Qt::Vertical,
               (m_effectiveGeometry.itemRect.top() + m_effectiveGeometry.itemRect.bottom()) / 2, m_effectiveGeometry.verticalCenterOffset,
               QStringLiteral("offset: %1px").arg(m_effectiveGeometry.verticalCenterOffset / m_zoom));
  }

  if (m_effectiveGeometry.bottom) {
    drawAnchor(p, Qt::Vertical,
               m_effectiveGeometry.itemRect.bottom(), -m_effectiveGeometry.bottomMargin,
               QStringLiteral("margin: %1px").arg(m_effectiveGeometry.bottomMargin / m_zoom));
  }

  if (m_effectiveGeometry.baseline) {
    drawAnchor(p, Qt::Vertical,
               m_effectiveGeometry.itemRect.top(), m_effectiveGeometry.baselineOffset,
               QStringLiteral("offset: %1px").arg(m_effectiveGeometry.baselineOffset / m_zoom));
  }

  p->restore();
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

void QuickScenePreviewWidget::drawRuler(QPainter* p)
{
    p->save();

    const int hRulerHeight = 35;
    const int vRulerWidth = 50;
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

        if (x % 100 == 0 && x >= 0 && x <= m_image.width() * m_zoom) {
            auto nearestTick = qRound(x / m_zoom * 0.2) * 5;
            auto xOffset = (nearestTick - (x/m_zoom)) * m_zoom;
            p->drawText(m_x + x + xOffset - 20, height() - (hRulerHeight - 2 * lineLength),
                        40, hRulerHeight - 2 * lineLength,
                        Qt::AlignHCenter | Qt::AlignVCenter, QString::number((int)nearestTick));
        }


        if (m_image.width() * m_zoom - x < 1) {
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

        if (y % 100 == 0 && y >= 0 && y <= m_image.height() * m_zoom) {
            auto nearestTick = qRound(y / m_zoom * 0.2) * 5;
            auto yOffset = (nearestTick - (y/m_zoom)) * m_zoom;
            p->drawText(width() - (vRulerWidth - 2 * lineLength), m_y + y + yOffset - 20,
                        vRulerWidth - 2 * lineLength, 40,
                        Qt::AlignHCenter | Qt::AlignVCenter, QString::number((int)nearestTick));
        }


        if (m_image.height() * m_zoom - y < 1) {
            p->setPen(inactivePen);
        }
    }

    p->setPen(activePen);
    p->drawText(QRect(width() - vRulerWidth, height() - hRulerHeight, vRulerWidth, hRulerHeight),
                QStringLiteral("%1x\n%2").arg(m_zoomedMousePosition.x()).arg(m_zoomedMousePosition.y()),
                Qt::AlignHCenter | Qt::AlignVCenter
               );

    p->restore();
}


void QuickScenePreviewWidget::drawArrow(QPainter *p, QPointF first, QPointF second)
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

void QuickScenePreviewWidget::drawAnchor(QPainter *p, Qt::Orientation orientation,
                                       qreal ownAnchorLine, qreal offset,
                                       const QString &label)
{
  qreal foreignAnchorLine = ownAnchorLine - offset;
  QPen pen(QColor(139, 179, 0));

  // Margin arrow
  if (offset) {
    p->setPen(pen);
    if (orientation == Qt::Horizontal) {
      drawArrow(p,
                QPointF(foreignAnchorLine, (m_effectiveGeometry.itemRect.top() + m_effectiveGeometry.itemRect.bottom()) / 2),
                QPointF(ownAnchorLine, (m_effectiveGeometry.itemRect.top() + m_effectiveGeometry.itemRect.bottom()) / 2));
    } else {
      drawArrow(p,
                QPointF((m_effectiveGeometry.itemRect.left() + m_effectiveGeometry.itemRect.right()) / 2, foreignAnchorLine),
                QPointF((m_effectiveGeometry.itemRect.left() + m_effectiveGeometry.itemRect.right()) / 2, ownAnchorLine));
    }

    // Margin text
    if (orientation == Qt::Horizontal) {
      p->drawText(
        QRectF(foreignAnchorLine,
               (m_effectiveGeometry.itemRect.top() + m_effectiveGeometry.itemRect.bottom()) / 2 + 10, offset, 50),
        Qt::AlignHCenter | Qt::TextDontClip,
        label);
    } else {
      p->drawText(
        QRectF((m_effectiveGeometry.itemRect.left() + m_effectiveGeometry.itemRect.right()) / 2 + 10,
               foreignAnchorLine, 100, offset),
        Qt::AlignVCenter | Qt::TextDontClip,
        label);
    }
  }

  // Own Anchor line
  pen.setWidth(2);
  p->setPen(pen);
  if (orientation == Qt::Horizontal) {
    p->drawLine(ownAnchorLine, m_effectiveGeometry.itemRect.top(), ownAnchorLine, m_effectiveGeometry.itemRect.bottom());
  } else {
    p->drawLine(m_effectiveGeometry.itemRect.left(), ownAnchorLine, m_effectiveGeometry.itemRect.right(), ownAnchorLine);
  }

  // Foreign Anchor line
  pen.setStyle(Qt::DotLine);
  p->setPen(pen);
  if (orientation == Qt::Horizontal) {
    p->drawLine(foreignAnchorLine, 0, foreignAnchorLine, m_image.height() * m_zoom);
  } else {
    p->drawLine(0, foreignAnchorLine, m_image.width() * m_zoom, foreignAnchorLine);
  }
}

void QuickScenePreviewWidget::drawMeasureLine(QPainter *p)
{
  p->save();

  p->setPen(QColor(0, 0, 0, 170));

  // We use zoomedMousePosition and "unzoom" it, by purpose, in order to snap it to the nearest pixel
  QPoint grabPos = QPoint(m_x, m_y) + m_zoomedMouseGrabPosition * m_zoom;
  QPoint pos = QPoint(m_x, m_y) + m_zoomedMousePosition * m_zoom;
  QPoint hOffset(5, 0);
  QPoint vOffset(0, 5);

  p->drawLine(grabPos - hOffset, grabPos + hOffset);
  p->drawLine(grabPos - vOffset, grabPos + vOffset);

  p->drawLine(grabPos, pos);

  p->drawLine(pos - hOffset, pos + hOffset);
  p->drawLine(pos - vOffset, pos + vOffset);

  p->restore();
}

void QuickScenePreviewWidget::mouseMoveEvent(QMouseEvent* e)
{
  m_zoomedMousePosition = (e->pos() - QPoint(m_x, m_y)) / m_zoom;

  if (m_mouseMode == MovePreview && e->buttons()) {
    m_x = e->x() - m_mouseGrabPosition.x();
    m_y = e->y() - m_mouseGrabPosition.y();

    if (m_x > width() / 2) {
      m_x = width() / 2;
    } else if (m_x + m_image.width() * m_zoom < width() / 2.0) {
      m_x = width() / 2 - m_image.width() * m_zoom;
    }
    if (m_y > height() / 2) {
      m_y = height() / 2;
    } else if (m_y + m_image.height() * m_zoom < height() / 2.0) {
      m_y = height() / 2 - m_image.height() * m_zoom;
    }
  } else if (m_mouseMode == MeasurePixels && e->buttons()) {
    int x = m_zoomedMouseGrabPosition.x() - m_zoomedMousePosition.x(); // the division is on integer-precision by purpose
    int y = m_zoomedMouseGrabPosition.y() - m_zoomedMousePosition.y();
    m_toolBar.measureLabel->setText(QStringLiteral(" %1px").arg(std::sqrt((qreal)(x*x + y*y)), 0, 'f', 2));
  } else if (m_mouseMode == RedirectInput) { // here we want hover also
    m_inspectorInterface->sendMouseEvent(e->type(), (e->pos() - QPoint(m_x, m_y)) / m_zoom, e->button(), e->buttons(), e->modifiers());
  }

  update();
}

void QuickScenePreviewWidget::mousePressEvent(QMouseEvent* e)
{
  m_mousePressed = true;
  m_mouseGrabPosition = e->pos() - QPoint(m_x, m_y);

  m_zoomedMouseGrabPosition = (e->pos() - QPoint(m_x, m_y)) / m_zoom;

  if (m_mouseMode == RedirectInput) {
    m_inspectorInterface->sendMouseEvent(e->type(), (e->pos() - QPoint(m_x, m_y)) / m_zoom, e->button(), e->buttons(), e->modifiers());
  }
}

void QuickScenePreviewWidget::mouseReleaseEvent(QMouseEvent* e)
{
  m_mousePressed = false;
  m_mouseGrabPosition = QPoint();
  m_toolBar.measureLabel->setText(QString());
  update();

  if (m_mouseMode == RedirectInput) {
    m_inspectorInterface->sendMouseEvent(e->type(), (e->pos() - QPoint(m_x, m_y)) / m_zoom, e->button(), e->buttons(), e->modifiers());
  }
}

void QuickScenePreviewWidget::keyPressEvent(QKeyEvent* e)
{
  if (m_mouseMode == RedirectInput) {
    m_inspectorInterface->sendKeyEvent(e->type(), e->key(), e->modifiers(), e->text(), e->isAutoRepeat(), e->count());
  }
}

void QuickScenePreviewWidget::keyReleaseEvent(QKeyEvent* e)
{
  if (m_mouseMode == RedirectInput) {
    m_inspectorInterface->sendKeyEvent(e->type(), e->key(), e->modifiers(), e->text(), e->isAutoRepeat(), e->count());
  }
}

void QuickScenePreviewWidget::wheelEvent(QWheelEvent* e)
{
  if (m_mouseMode == RedirectInput) {
    m_inspectorInterface->sendWheelEvent((e->pos() - QPoint(m_x, m_y)) / m_zoom, e->pixelDelta(), e->angleDelta(), e->buttons(), e->modifiers());
  }
}

void QuickScenePreviewWidget::setImage(const QImage& image)
{
  if (m_image.isNull()) {
    m_x = 0.5 * (width() - image.width());
    m_y = 0.5 * (height() - image.height());
  }
  m_image = image;
  update();
}

void QuickScenePreviewWidget::setItemGeometry(const QuickItemGeometry &itemGeometry)
{
  m_itemGeometry = itemGeometry;

  updateEffectiveGeometry();
  update();
}

void QuickScenePreviewWidget::updateEffectiveGeometry()
{
  m_effectiveGeometry = m_itemGeometry;

  m_effectiveGeometry.itemRect               = QRectF(m_itemGeometry.itemRect.topLeft() * m_zoom, m_itemGeometry.itemRect.bottomRight() * m_zoom);
  m_effectiveGeometry.boundingRect           = QRectF(m_itemGeometry.boundingRect.topLeft() * m_zoom, m_itemGeometry.boundingRect.bottomRight() * m_zoom);
  m_effectiveGeometry.childrenRect           = QRectF(m_itemGeometry.childrenRect.topLeft() * m_zoom, m_itemGeometry.childrenRect.bottomRight() * m_zoom);
  m_effectiveGeometry.transformOriginPoint   = m_itemGeometry.transformOriginPoint * m_zoom;
  m_effectiveGeometry.leftMargin             = m_itemGeometry.leftMargin * m_zoom;
  m_effectiveGeometry.horizontalCenterOffset = m_itemGeometry.horizontalCenterOffset * m_zoom;
  m_effectiveGeometry.rightMargin            = m_itemGeometry.rightMargin * m_zoom;
  m_effectiveGeometry.topMargin              = m_itemGeometry.topMargin * m_zoom;
  m_effectiveGeometry.verticalCenterOffset   = m_itemGeometry.verticalCenterOffset * m_zoom;
  m_effectiveGeometry.bottomMargin           = m_itemGeometry.bottomMargin * m_zoom;
  m_effectiveGeometry.baselineOffset         = m_itemGeometry.baselineOffset * m_zoom;
  m_effectiveGeometry.x                      = m_itemGeometry.x * m_zoom;
  m_effectiveGeometry.y                      = m_itemGeometry.y * m_zoom;
}

void QuickScenePreviewWidget::setZoom(qreal zoom)
{
  m_x = width() / 2 - (width() / 2 - m_x) * zoom / m_zoom ;
  m_y = height() / 2 - (height() / 2 - m_y) * zoom / m_zoom ;
  m_zoom = zoom;
  updateEffectiveGeometry();
  update();
}

void QuickScenePreviewWidget::setZoomFromCombobox(int)
{
  setZoom(m_toolBar.zoomCombobox->currentData().toReal());
}

void QuickScenePreviewWidget::visualizeActionTriggered(bool checked)
{
  static bool recursionGuard = false;
  if (recursionGuard)
      return;

  if (!checked) {
    m_inspectorInterface->setCustomRenderMode(QuickInspectorInterface::NormalRendering);
  } else {
    // QActionGroup requires exactly one selected, but we need 0 or 1 selected
    const auto current = sender();
    recursionGuard = true;
    foreach (auto action, m_toolBar.visualizeGroup->actions()) {
      if (action != current)
        action->setChecked(false);
    }
    recursionGuard = false;
    m_inspectorInterface->setCustomRenderMode(current == m_toolBar.visualizeClipping ? QuickInspectorInterface::VisualizeClipping
                                            : current == m_toolBar.visualizeBatches ? QuickInspectorInterface::VisualizeBatches
                                            : current == m_toolBar.visualizeOverdraw ? QuickInspectorInterface::VisualizeOverdraw
                                            : current == m_toolBar.visualizeChanges ? QuickInspectorInterface::VisualizeChanges
                                            : QuickInspectorInterface::NormalRendering
    );
  }
}

void QuickScenePreviewWidget::setMouseTool(QAction *action)
{
  m_mouseMode = action == m_toolBar.measurePixels ? MeasurePixels
              : action == m_toolBar.redirectInput ? RedirectInput
              : MovePreview;
}

void GammaRay::QuickScenePreviewWidget::setSupportsCustomRenderModes(QuickInspectorInterface::Features supportedCustomRenderModes)
{
  m_toolBar.visualizeClipping->setEnabled(supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeClipping);
  m_toolBar.visualizeBatches->setEnabled(supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeBatches);
  m_toolBar.visualizeOverdraw->setEnabled(supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeOverdraw);
  m_toolBar.visualizeChanges->setEnabled(supportedCustomRenderModes & QuickInspectorInterface::CustomRenderModeChanges);
}
