/*
  overlaywidget.cpp

  This file is part of GammaRay, the Qt application inspection and
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
#include <QtGui/QDialog>
#include <QtGui/QLayout>
#include <QtGui/QPainter>

using namespace GammaRay;

static QWidget *toplevelWidget(QWidget *widget)
{
  QWidget *parent = widget;
  while (parent->parentWidget() &&
         (qobject_cast<QDialog*>(parent->parentWidget()) == 0) &&
         (qobject_cast<QDialog*>(parent) == 0)) {
    parent = parent->parentWidget();
  }

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
    if (m_currentWidget) {
      m_currentWidget->removeEventFilter(this);
    }

    if (m_currentToplevelWidget) {
      m_currentToplevelWidget->removeEventFilter(this);
    }

    m_currentToplevelWidget = 0;
    m_currentWidget = 0;
    m_widgetRect = QRect();
    m_layoutPath = QPainterPath();

    update();
    return;
  }

  QWidget *toplevel = toplevelWidget(widget);
  Q_ASSERT(toplevel);

  if (m_currentWidget) {
    m_currentWidget->removeEventFilter(this);
  }

  m_currentWidget = widget;

  if (toplevel != m_currentToplevelWidget) {
    if (m_currentToplevelWidget) {
      m_currentToplevelWidget->removeEventFilter(this);
    }

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
      updatePositions();
    }
  } else if (receiver == m_currentWidget) {
    if (event->type() == QEvent::Resize || event->type() == QEvent::Move) {
      resizeOverlay();
      updatePositions();
    }
  }

  return false;
}

void OverlayWidget::updatePositions()
{
  if (!m_currentWidget || !m_currentToplevelWidget) {
    return;
  }

  if (!m_currentWidget->isVisible() || m_currentWidget->isHidden()) {
    m_widgetColor = Qt::green;
  } else {
    m_widgetColor = Qt::red;
  }

  const QPoint parentPos = m_currentWidget->mapTo(m_currentToplevelWidget, QPoint(0, 0));
  m_widgetRect = QRect(parentPos.x(), parentPos.y(),
                       m_currentWidget->width(),
                       m_currentWidget->height()).adjusted(0, 0, -1, -1);

  m_layoutPath = QPainterPath();

  if (m_currentWidget->layout() &&
      qstrcmp(m_currentWidget->layout()->metaObject()->className(), "QMainWindowLayout") != 0) {
    const QRect layoutGeometry = m_currentWidget->layout()->geometry();

    const QRect mappedOuterRect =
      QRect(m_currentWidget->mapTo(m_currentToplevelWidget,
                                   layoutGeometry.topLeft()), layoutGeometry.size());

    QPainterPath outerPath;
    outerPath.addRect(mappedOuterRect.adjusted(1, 1, -2, -2));

    QPainterPath innerPath;
    for (int i = 0; i < m_currentWidget->layout()->count(); ++i) {
      QLayoutItem *item = m_currentWidget->layout()->itemAt(i);
      const QRect mappedInnerRect =
        QRect(m_currentWidget->mapTo(m_currentToplevelWidget,
                                     item->geometry().topLeft()), item->geometry().size());
      innerPath.addRect(mappedInnerRect);
    }

    m_layoutPath.setFillRule(Qt::OddEvenFill);
    m_layoutPath = outerPath.subtracted(innerPath);

    if (m_layoutPath.isEmpty()) {
      m_layoutPath = outerPath;
      m_layoutPath.addPath(innerPath);
      m_drawLayoutOutlineOnly = true;
    } else {
      m_drawLayoutOutlineOnly = false;
    }
  }

  update();
}

void OverlayWidget::resizeOverlay()
{
  if (m_currentToplevelWidget) {
    move(0, 0);
    resize(m_currentToplevelWidget->size());
  }
}

void OverlayWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.setPen(m_widgetColor);
  p.drawRect(m_widgetRect);

  QBrush brush(Qt::BDiagPattern);
  brush.setColor(Qt::blue);

  if (!m_drawLayoutOutlineOnly) {
    p.fillPath(m_layoutPath, brush);
  }

  p.setPen(Qt::blue);
  p.drawPath(m_layoutPath);
}
