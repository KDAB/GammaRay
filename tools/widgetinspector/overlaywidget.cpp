/*
  overlaywidget.cpp

  This file is part of Gammaray, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tobias Koenig <tobias.koenig@kdab.com>

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

#include "overlaywidget.h"

#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>

using namespace Gammaray;

static QWidget* toplevelWidget(QWidget *widget)
{
  QWidget *parent = widget;
  while (parent->parentWidget())
    parent = parent->parentWidget();

  return parent;
}

OverlayWidget::OverlayWidget()
  : m_currentToplevelWidget(0),
    m_currentWidget(0)
{
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setFocusPolicy(Qt::NoFocus);
}

void OverlayWidget::placeOn(QWidget *widget)
{
  if (widget == 0) {
    if (m_currentWidget)
      m_currentWidget->removeEventFilter(this);

    if (m_currentToplevelWidget)
      m_currentToplevelWidget->removeEventFilter(this);
    
    m_currentToplevelWidget = 0;
    m_currentWidget = 0;
    m_rect = QRect();

    update();
    return;
  }

  QWidget *toplevel = toplevelWidget(widget);
  Q_ASSERT(toplevel);

  if (m_currentWidget)
    m_currentWidget->removeEventFilter(this);

  m_currentWidget = widget;

  if (toplevel != m_currentToplevelWidget) {
    if (m_currentToplevelWidget)
      m_currentToplevelWidget->removeEventFilter(this);

    m_currentToplevelWidget = toplevel;

    setParent(toplevel);
    move(0, 0);
    resize(toplevel->size());

    m_currentToplevelWidget->installEventFilter(this);

    show();
  }

  m_currentWidget->installEventFilter(this);

  updatePositions();
}

bool OverlayWidget::eventFilter(QObject *receiver, QEvent *event)
{
  if (receiver == m_currentToplevelWidget) {
    if (event->type() == QEvent::Resize) {
      resizeOverlay();
    }
  } else if (receiver == m_currentWidget) {
    if (event->type() == QEvent::Resize || event->type() == QEvent::Move) {
      updatePositions();
    }
  }

  return false;
}

void OverlayWidget::updatePositions()
{
  if (!m_currentWidget || !m_currentToplevelWidget)
    return;

  if (!m_currentWidget->isVisible() || m_currentWidget->isHidden()) {
    m_rect = QRect();
  } else {
    const QPoint parentPos = m_currentWidget->mapTo(m_currentToplevelWidget, QPoint(0, 0));
    m_rect = QRect(parentPos.x(), parentPos.y(), m_currentWidget->width(), m_currentWidget->height()).adjusted(0, 0, -1, -1);
  }

  update();
}

void OverlayWidget::resizeOverlay()
{
  if (m_currentToplevelWidget) {
    move(0, 0);
    resize(m_currentToplevelWidget->size());

    updatePositions();
  }
}

void OverlayWidget::paintEvent(QPaintEvent*)
{
  QPainter p(this);
  p.setPen(Qt::red);
  p.drawRect(m_rect);
}
