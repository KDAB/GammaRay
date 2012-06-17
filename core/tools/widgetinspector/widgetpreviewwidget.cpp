/*
  widgetpreviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
  : QWidget(parent), m_grabbingWidget(false)
{
}

void WidgetPreviewWidget::setWidget(QWidget *widget)
{
  if (m_widget) {
    m_widget.data()->removeEventFilter(this);
  }
  if (widget != this) {
    m_widget = widget;
  } else {
    m_widget = 0;
  }
  if (widget) {
    widget->installEventFilter(this);
  }
  update();
}

void WidgetPreviewWidget::paintEvent(QPaintEvent *event)
{
  if (m_widget && !m_grabbingWidget) {
    m_grabbingWidget = true;
    const QPixmap pixmap = QPixmap::grabWidget(m_widget.data());
    m_grabbingWidget = false;
    QPainter painter(this);
    const qreal scale = qMin<qreal>(1.0, qMin((qreal)width() / (qreal)pixmap.width(),
                                              (qreal)height() / (qreal)pixmap.height()));
    const qreal targetWidth = pixmap.width() * scale;
    const qreal targetHeight = pixmap.height() * scale;
    painter.drawPixmap((width() - targetWidth) / 2,
                       (height() - targetHeight) / 2,
                       targetWidth, targetHeight,
                       pixmap);
  }
  QWidget::paintEvent(event);
}

bool WidgetPreviewWidget::eventFilter(QObject *receiver, QEvent *event)
{
  if (!m_grabbingWidget && receiver == m_widget.data() && event->type() == QEvent::Paint) {
    update();
  }
  return QWidget::eventFilter(receiver, event);
}

#include "widgetpreviewwidget.moc"
