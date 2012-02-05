/*
  paintbufferreplaywidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "paintbufferreplaywidget.h"

#ifdef HAVE_PRIVATE_QT_HEADERS
using namespace GammaRay;

PaintBufferReplayWidget::PaintBufferReplayWidget(QWidget* parent): QWidget(parent), m_endCommandIndex(0)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void PaintBufferReplayWidget::setPaintBuffer(const QPaintBuffer& buffer)
{
  m_buffer = buffer;
  update();
}

void PaintBufferReplayWidget::setEndCommandIndex(int index)
{
  m_endCommandIndex = index;
  update();
}

QSize PaintBufferReplayWidget::sizeHint() const
{
  return m_buffer.boundingRect().size().toSize();
}

// TODO: factor out into util namespace, similar code exists in the style tool
void PaintBufferReplayWidget::drawTransparencyPattern(QPainter* painter, const QRect& rect, int squareSize)
{
  QPixmap bgPattern(2*squareSize, 2*squareSize);
  bgPattern.fill(Qt::lightGray);
  QPainter bgPainter(&bgPattern);
  bgPainter.fillRect(squareSize, 0, squareSize, squareSize, Qt::gray);
  bgPainter.fillRect(0, squareSize, squareSize, squareSize, Qt::gray);

  QBrush bgBrush;
  bgBrush.setTexture(bgPattern);
  painter->fillRect(rect, bgBrush);
}

void PaintBufferReplayWidget::paintEvent(QPaintEvent* event)
{
  // didn't manage painting on the widget directly, even with the correct translation it is always clipping as if the widget was at 0,0 of its parent
  QImage img(sizeHint(), QImage::Format_ARGB32);
  QPainter imgPainter(&img);
  drawTransparencyPattern(&imgPainter, QRect(QPoint(0,0), sizeHint()));
  int depth = m_buffer.processCommands(&imgPainter, m_buffer.frameStartIndex(0), m_buffer.frameStartIndex(0) + m_endCommandIndex);
  for (;depth > 0;--depth)
    imgPainter.restore();
  imgPainter.end();

  QPainter p(this);
  p.drawImage(QPoint(0,0), img);
}

#include "paintbufferreplaywidget.moc"
#endif
