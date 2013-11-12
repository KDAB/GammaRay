/*
  widgetpreviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "widgetpreviewwidget.h"

#include <QEvent>
#include <QPainter>

using namespace GammaRay;

WidgetPreviewWidget::WidgetPreviewWidget(QWidget *parent)
  : QWidget(parent)
{
}

void WidgetPreviewWidget::setPixmap(const QPixmap &pixmap)
{
  m_pixmap = pixmap;
  update();
}

void WidgetPreviewWidget::paintEvent(QPaintEvent * /*event*/)
{
  QPainter painter(this);
  const qreal scale = qMin<qreal>(1.0, qMin((qreal)width() / (qreal)m_pixmap.width(),
                                            (qreal)height() / (qreal)m_pixmap.height()));
  const qreal targetWidth = m_pixmap.width() * scale;
  const qreal targetHeight = m_pixmap.height() * scale;
  painter.drawPixmap((width() - targetWidth) / 2,
                      (height() - targetHeight) / 2,
                      targetWidth, targetHeight,
                      m_pixmap);
}
