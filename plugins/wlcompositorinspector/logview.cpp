/*
  logview.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

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

#include "logview.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QStaticText>
#include <QPainter>
#include <QScrollArea>
#include <QClipboard>
#include <QApplication>

#include "ringbuffer.h"

namespace GammaRay {

class LogView::View : public QWidget
{
public:
  View(QWidget *p)
    : QWidget(p)
    , m_lines(500)
    , m_metrics(QFont())
    , m_lineHeight(m_metrics.height())
  {
    resize(0, 0);
    setFocusPolicy(Qt::ClickFocus);
  }

  QSize sizeHint() const override
  {
    return size();
  }

  void drawLine(QPainter &painter, const QRect &rect, const QStaticText &line)
  {
    painter.setPen(palette().color(QPalette::Text));
    painter.drawStaticText(0, rect.y(), line);
  }

  void drawLineSelected(QPainter &painter, const QRect &rect, const QStaticText &line)
  {
    painter.fillRect(rect, palette().highlight());
    painter.setPen(palette().color(QPalette::HighlightedText));
    painter.drawStaticText(0, rect.y(), line);
  }

  void drawLinePartialSelected(QPainter &painter, const QRect &rect, const QStaticText &line, int startSelectChar, int endSelectChar)
  {
    const QString &text = line.text();
    int startX = m_metrics.width(text.left(startSelectChar));
    int endX = m_metrics.width(text.left(endSelectChar));

    if (startSelectChar > 0) {
      painter.drawText(QRect(rect.x(), rect.y(), startX,  rect.height()), Qt::TextDontClip, text.left(startSelectChar));
    }

    QRect selectRect(rect.x() + startX, rect.y(), endX - startX, rect.height());
    painter.fillRect(selectRect, palette().highlight());
    painter.setPen(palette().color(QPalette::HighlightedText));
    painter.drawText(selectRect, Qt::TextDontClip, text.mid(startSelectChar, endSelectChar - startSelectChar));

    if (endSelectChar < text.count()) {
      painter.setPen(palette().color(QPalette::Text));
      painter.drawText(QRect(rect.x() + endX, rect.y(), m_metrics.width(text) - endX, rect.height()), text.mid(endSelectChar));
    }
  }

  void selectionBoundaries(QPoint &start, QPoint &end) const
  {
    bool startBeforeEnd = (m_selectionStart.y() < m_selectionEnd.y()) ||
                          (m_selectionStart.y() == m_selectionEnd.y() && m_selectionStart.x() < m_selectionEnd.x());
    start = startBeforeEnd ? m_selectionStart : m_selectionEnd;
    end = startBeforeEnd ? m_selectionEnd : m_selectionStart;
  }

  struct LineSelection
  {
    bool isNull() const { return start == end; }
    bool isFull() const { return start == 0 && end < 0; }

    int start;
    int end;
  };
  LineSelection lineSelection(int line) const
  {
    if (m_selectionStart == m_selectionEnd) {
      return { 0, 0 };
    }

    QPoint start, end;
    selectionBoundaries(start, end);

    if (start.y() < line && line < end.y()) {
      return { 0, m_lines.at(line).text().count() };
    }

    if (start.y() == line || end.y() == line) {
      int startChar = 0;
      int endChar = m_lines.at(line).text().count();
      if (start.y() == line)
        startChar = start.x();
      if (end.y() == line)
        endChar = end.x() + 1;
      return { startChar, endChar };
    }

    return { 0, 0 };
  }

  void paintEvent(QPaintEvent *event) override
  {
    if (m_lineHeight < 0) {
      return;
    }

    QPainter painter(this);

    QRectF drawRect = event->rect();
    int startingLine = lineAt(drawRect.y());
    int y = m_lineHeight * startingLine;

    for (int i = startingLine; i < m_lines.count(); ++i) {
      const QStaticText &text = m_lines.at(i);

      QRect lineRect(QRect(0, y, text.size().width(), m_lineHeight));
      painter.fillRect(QRectF(0, y, drawRect.width(), m_lineHeight), i % 2 ? palette().base() : palette().alternateBase());

      LineSelection selection = lineSelection(i);
      if (selection.isNull()) {
        drawLine(painter, lineRect, text);
      } else if (selection.isFull()) {
        drawLineSelected(painter, lineRect, text);
      } else {
        drawLinePartialSelected(painter, lineRect, text, selection.start, selection.end);
      }

      y += m_lineHeight;
      if (y >= drawRect.bottom())
        break;
    }
  }

  inline int lineAt(int y) const { return qMin(y / m_lineHeight, m_lines.count() - 1); }
  inline QPoint charPosAt(const QPointF &p) const
  {
    int line = lineAt(p.y());
    int lineX = 0;

    const QString &text = m_lines.at(line).text();
    for (int x = 0, i = 0; i < text.count(); ++i) {
      const QChar &c = text.at(i);
      if (p.x() >= x) {
        lineX = i;
      }
      x += m_metrics.width(c);
    }

    return QPoint(lineX, line);
  }

  void mousePressEvent(QMouseEvent *e)
  {
    if (e->button() == Qt::LeftButton) {
      m_selectionStart = m_selectionEnd = charPosAt(e->pos());
      e->accept();
      update();
    }
  }

  void mouseMoveEvent(QMouseEvent *e)
  {
    m_selectionEnd = charPosAt(e->pos());
    e->accept();
    update();
  }

  QString selectedText() const
  {
    if (m_selectionStart == m_selectionEnd) {
      return QString();
    }

    QPoint start, end;
    selectionBoundaries(start, end);
    QString string;
    for (int i = start.y(); i <= end.y(); ++i) {
      const QStaticText &line = m_lines.at(i);
      LineSelection selection = lineSelection(i);
      string += line.text().mid(selection.start, selection.end - selection.start);
      string += QLatin1Char('\n');
    }
    return string;
  }

  void keyPressEvent(QKeyEvent *e)
  {
    if (e->key() == Qt::Key_C && e->modifiers() == Qt::ControlModifier) {
      QApplication::clipboard()->setText(selectedText());
    }
  }

  RingBuffer<QStaticText> m_lines;
  QFontMetricsF m_metrics;
  int m_lineHeight;
  QPoint m_selectionStart;
  QPoint m_selectionEnd;
};

LogView::LogView(QWidget *p)
       : QScrollArea(p)
       , m_view(new View(this))
{
  m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setWidget(m_view);
  setWidgetResizable(true);
}

QSize LogView::sizeHint() const
{
  return QSize(200, 200);
}

void LogView::logMessage(const QByteArray &msg)
{
  auto scrollbar = verticalScrollBar();
  bool scroll = scrollbar->value() >= scrollbar->maximum();

  add(msg);

  if (scroll)
    scrollbar->setValue(scrollbar->maximum());
}

void LogView::reset()
{
  m_view->m_lines.clear();
  m_view->resize(0, 0);
}

void LogView::add(const QByteArray &m)
{
  int count = m_view->m_lines.count();
  m_view->m_lines.append(QStaticText(m));

  QSizeF lineSize = m_view->m_lines.last().size();

  int w = m_view->width();
  int h = m_view->height();
  if (m_view->m_lines.count() > count) {
    h += m_view->m_lineHeight;
  }
  if (lineSize.width() > w) {
    w = lineSize.width();
  }
  m_view->resize(w, h);
  m_view->update();
}

}
