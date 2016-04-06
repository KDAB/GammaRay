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
#include <QtMath>

#include "ringbuffer.h"

namespace GammaRay {

class View : public QWidget
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
    setCursor(Qt::IBeamCursor);
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


class Messages : public QScrollArea
{
public:
  Messages(QWidget *parent)
    : QScrollArea(parent)
    , m_view(new View(this))
  {
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setWidget(m_view);
    setWidgetResizable(true);
  }

  void logMessage(qint64 time, const QByteArray &msg)
  {
    auto scrollbar = verticalScrollBar();
    bool scroll = scrollbar->value() >= scrollbar->maximum();

    add(time, msg);

    if (scroll)
      scrollbar->setValue(scrollbar->maximum());
  }

  void reset()
  {
    m_view->m_lines.clear();
    m_view->resize(0, 0);
  }

  void add(qint64 time, const QByteArray &m)
  {
    int count = m_view->m_lines.count();
    m_view->m_lines.append(QStaticText(QString("[%1ms] %2").arg(QString::number(time / 1e6), QString(m))));

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

  View *m_view;
};


class Timeline : public QScrollArea
{
public:
  class View : public QWidget
  {
  public:
    struct DataPoint {
      qint64 time;
      QByteArray msg;
    };

    View()
      : m_data(500)
      , m_zoom(100000)
    {
      resize(100, 100);
      setAttribute(Qt::WA_OpaquePaintEvent);
      setMouseTracking(true);
    }

    QSize sizeHint() const override
    {
      return size();
    }

    void paintEvent(QPaintEvent *event) override
    {
      QPainter painter(this);
      QRectF drawRect = event->rect();
      const auto palette = this->palette();

      painter.fillRect(drawRect, palette.base());

      qreal l = 1;
      qreal step = l / m_zoom;
      while (step < 60) {
          l *= 10;
          step = l / m_zoom;
      }

      int substeps = 5;
      int mul = 2;
      while (step / substeps > 60) {
        substeps *= mul;
        mul = mul == 2 ? 5 : 2;
      }

      //draw the grid lines
      qreal linesSpacing = step / substeps;
      int startLine = drawRect.left() / linesSpacing;
      int s = startLine;
      for (qreal i = startLine * linesSpacing; i < drawRect.right(); i += linesSpacing, s++) {
        bool isStep = s % substeps == 0;
        painter.setPen(isStep ? palette.color(QPalette::Highlight) : palette.color(QPalette::Midlight));
        painter.drawLine(i, isStep * 15, i, drawRect.bottom());
      }

      //draw the text after having drawn all the lines, so we're sure they don't go over it
      s = startLine;
      painter.setPen(palette.color(QPalette::Highlight));
      for (qreal i = startLine * linesSpacing; i < drawRect.right(); i += step / substeps, s++) {
        bool isStep = s % substeps == 0;
        if (isStep) {
          painter.drawText(i-100, 0, 200, 200, Qt::AlignHCenter, QString("%1ms").arg(QString::number(qreal(i * m_zoom) / 1e6, 'g', 4)));
        }
      }

      //finally draw the event lines
      painter.setPen(palette.color(QPalette::Text));
      bool hasDrawn = false;
      for (int i = 0; i < m_data.count(); ++i) {
        const auto &point = m_data.at(i);
        qreal offset = point.time - m_start;
        qreal x = offset / m_zoom;
        qreal y = qMax(20., drawRect.y());
        if (!drawRect.contains(QPoint(x, y))) {
          if (hasDrawn)
            break;
          else
            continue;
        }
        hasDrawn = true;

        painter.drawLine(x, y, x, drawRect.bottom());
      }
    }

    void mouseMoveEvent(QMouseEvent *e) override
    {
      const QPointF &pos = e->posF();
      for (int i = 0; i < m_data.count(); ++i) {
        qreal timex = (m_data.at(i).time - m_start) / m_zoom;
        if (fabs(pos.x() - timex) < 2) {
          setToolTip(m_data.at(i).msg);
          return;
        }
      }
    }

    qint64 round(qint64 time, int direction)
    {
      qint64 v = time % 200;
      return time + direction * v;
    }

    void updateSize()
    {
      if (m_data.count() == 0)
        return;

      m_start = round(m_data.at(0).time, -1);
      m_timespan = round(m_data.last().time, 1) - m_start;
      resize(m_timespan / m_zoom, height());
    }

    RingBuffer<DataPoint> m_data;
    qreal m_zoom;
    qint64 m_start;
    qint64 m_timespan;
  };

  Timeline(QWidget *parent)
    : QScrollArea(parent)
  {
    m_view.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setWidget(&m_view);
    setWidgetResizable(true);
    m_view.installEventFilter(this);
  }

  void logMessage(qint64 time, const QByteArray &msg)
  {
    m_view.m_data.append({ time, msg });
    m_view.updateSize();
  }

  bool eventFilter(QObject *o, QEvent *e) override
  {
    if (o == &m_view && e->type() == QEvent::Wheel) {
      QWheelEvent *we = static_cast<QWheelEvent *>(e);

      qreal pos = we->posF().x() * m_view.m_zoom;
      auto sb = horizontalScrollBar();
      int sbvalue = horizontalScrollBar()->value();

      m_view.m_zoom += (1. - qPow( 5. / 4., qreal(we->angleDelta().y()) / 150.)) * m_view.m_zoom;
      if (m_view.m_zoom < 10) {
        m_view.m_zoom = 10;
      }

      m_view.updateSize();

      //keep the point under the mouse still, if possible
      pos = pos / m_view.m_zoom;
      sb->setValue(sbvalue + (0.5 + pos - we->posF().x()));
    }
    return QScrollArea::eventFilter(o, e);
  }

  View m_view;
};

LogView::LogView(QWidget *p)
       : QTabWidget(p)
       , m_messages(new Messages(this))
       , m_timeline(new Timeline(this))
{
  setTabPosition(QTabWidget::West);
  addTab(m_messages, tr("Messages"));
  addTab(m_timeline, tr("Timeline"));
}

QSize LogView::sizeHint() const
{
  return QSize(200, 200);
}

void LogView::logMessage(qint64 time, const QByteArray &msg)
{
  m_messages->logMessage(time, msg);
  m_timeline->logMessage(time, msg);
}

void LogView::reset()
{
  m_messages->reset();
}

}
