/*
  logview.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
  explicit View(QWidget *p)
    : QWidget(p)
    , m_lines(5000)
    , m_metrics(QFont())
    , m_lineHeight(m_metrics.height())
    , m_client(0)
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
      return { 0, m_lines.at(line).text.text().count() };
    }

    if (start.y() == line || end.y() == line) {
      int startChar = 0;
      int endChar = m_lines.at(line).text.text().count();
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
    int y = linePosAt(drawRect.y());

    for (int i = startingLine; i < m_lines.count(); ++i) {
      if (m_client && m_lines.at(i).pid != m_client) {
        continue;
      }
      const QStaticText &text = m_lines.at(i).text;

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

  inline int linesCount() const
  {
    return m_client ? m_linesCount.value(m_client) : m_lines.count();
  }

  inline int linePosAt(int y) const {
    int line = qMin(y / m_lineHeight, m_lines.count() - 1);
    return line * m_lineHeight;
  }

  inline int lineAt(int y) const {
    int line = qMin(y / m_lineHeight, m_lines.count() - 1);
    if (!m_client) {
      return line;
    }

    for (int i = 0, l = 0; i < m_lines.count(); ++i) {
      if (m_lines.at(i).pid == m_client) {
        if (l++ == line) {
          return i;
        }
      }
    }
    return line;
  }
  inline QPoint charPosAt(const QPointF &p) const
  {
    int line = lineAt(p.y());
    int lineX = 0;

    const QString &text = m_lines.at(line).text.text();
    for (int x = 0, i = 0; i < text.count(); ++i) {
      const QChar &c = text.at(i);
      if (p.x() >= x) {
        lineX = i;
      }
      x += m_metrics.width(c);
    }

    return {lineX, line};
  }

  void mousePressEvent(QMouseEvent *e) override
  {
    if (e->button() == Qt::LeftButton) {
      m_selectionStart = m_selectionEnd = charPosAt(e->pos());
      e->accept();
      update();
    }
  }

  void mouseMoveEvent(QMouseEvent *e) override
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
      if (m_client && m_lines.at(i).pid != m_client) {
        continue;
      }
      const QStaticText &line = m_lines.at(i).text;
      LineSelection selection = lineSelection(i);
      string += line.text().mid(selection.start, selection.end - selection.start);
      string += QLatin1Char('\n');
    }
    return string;
  }

  void keyPressEvent(QKeyEvent *e) override
  {
    if (e->key() == Qt::Key_C && e->modifiers() == Qt::ControlModifier) {
      QApplication::clipboard()->setText(selectedText());
    }
  }

  void resetSelection()
  {
      m_selectionStart = m_selectionEnd = QPoint();
      update();
  }

  struct Line {
      quint64 pid = 0;
      QStaticText text;
      int *counter = nullptr;

      Line() = default;

      Line(quint64 p, const QStaticText &t, int *cnt)
        : pid(p), text(t), counter(cnt)
      {
        (*counter)++;
      }

      Line(const Line &l)
        : pid(l.pid), text(l.text), counter(l.counter)
      {
        (*counter)++;
      }

      ~Line() { (*counter)--; }

      Line &operator=(const Line &l) {
          (*counter)--;

          pid = l.pid;
          text = l.text;
          counter = l.counter;

          (*counter)++;
          return *this;
      }

  };
  RingBuffer<Line> m_lines;
  QHash<quint64, int> m_linesCount;
  QFontMetricsF m_metrics;
  int m_lineHeight;
  QPoint m_selectionStart;
  QPoint m_selectionEnd;
  quint64 m_client;
};


class Messages : public QScrollArea
{
public:
  explicit Messages(QWidget *parent)
    : QScrollArea(parent)
    , m_view(new View(this))
  {
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setWidget(m_view);
    setWidgetResizable(true);
  }

  void logMessage(quint64 pid, qint64 time, const QByteArray &msg)
  {
    auto scrollbar = verticalScrollBar();
    bool scroll = scrollbar->value() >= scrollbar->maximum();

    add(pid, time, msg);

    if (scroll)
      scrollbar->setValue(scrollbar->maximum());
  }

  void reset()
  {
    m_view->m_lines.clear();
    m_view->resize(0, 0);
  }

  void updateSize()
  {
    QSizeF lineSize = m_view->m_lines.last().text.size();

    int w = m_view->width();
    int h = m_view->linesCount() * m_view->m_lineHeight;

    if (lineSize.width() > w) {
      w = lineSize.width();
    }
    m_view->resize(w, h);
    m_view->update();
  }

  void add(quint64 pid, qint64 time, const QByteArray &m)
  {
    m_view->m_lines.append(View::Line(pid, QStaticText(QString("[%1ms] %2").arg(QString::number(time / 1e6), QString(m))), &m_view->m_linesCount[pid]));

    if (m_view->m_client && pid != m_view->m_client) {
      return;
    }

    updateSize();
  }

  void setLoggingClient(quint64 pid)
  {
    m_view->m_client = pid;

    auto scrollbar = verticalScrollBar();
    qreal v = (qreal)scrollbar->value() / (qreal)scrollbar->maximum();

    m_view->resetSelection();
    updateSize();

    // keep the scrollbar at he same percentage
    scrollbar->setValue(v * (qreal)scrollbar->maximum());
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
      quint64 pid;
      QByteArray msg;
    };

    View()
      : m_data(5000)
    {
      resize(100, 100);
      setAttribute(Qt::WA_OpaquePaintEvent);
      setMouseTracking(true);
    }

    QSize sizeHint() const override
    {
      return size();
    }

    inline qint64 initialTime() const { return m_data.isEmpty() ? 0 : m_data.at(0).time; }

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

      auto it = initialTime();
      auto rit = round(it, -1);

      //draw the grid lines
      qreal linesSpacing = step / substeps;
      int startLine = drawRect.left() / linesSpacing - (rit - it) / m_zoom; //round the starting position so that we have nice numbers'

      int s = startLine;
      for (qreal i = startLine * linesSpacing; i < drawRect.right(); i += linesSpacing, s++) {
        bool isStep = s % substeps == 0;
        painter.setPen(isStep ? palette.color(QPalette::Highlight) : palette.color(QPalette::Midlight));

        int y = 0;
        if (isStep) {
          int stepN = s / substeps;
          y = 15 * (stepN % 2 + 1);
        }

        painter.drawLine(i, y, i, drawRect.bottom());
      }

      //draw the text after having drawn all the lines, so we're sure they don't go over it
      s = startLine;
      painter.setPen(palette.color(QPalette::Highlight));
      for (qreal i = startLine * linesSpacing; i < drawRect.right(); i += step / substeps, s++) { //krazy:exclude=postfixop
        bool isStep = s % substeps == 0;
        if (isStep) {
          painter.drawText(i-100, ((s / substeps) % 2) * 15, 200, 200, Qt::AlignHCenter, QString("%1ms").arg(QString::number(qreal(it + i * m_zoom) / 1e6, 'g', 6)));
        }
      }

      //finally draw the event lines
      painter.setPen(palette.color(QPalette::Text));
      bool hasDrawn = false;
      for (int i = 0; i < m_data.count(); ++i) {
        const auto &point = m_data.at(i);
        if (m_client && point.pid != m_client) {
            painter.setPen(palette.color(QPalette::Dark));
        } else {
            painter.setPen(palette.color(QPalette::Text));
        }

        qreal offset = point.time - m_start;
        qreal x = offset / m_zoom;
        qreal y = qMax(qreal(40.), drawRect.y());
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
      const QPointF &pos = e->localPos();
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
    qreal m_zoom = 100000;
    qint64 m_start = 0;
    qint64 m_timespan = 0;
    quint64 m_client = 0;
  };

  explicit Timeline(QWidget *parent)
    : QScrollArea(parent)
  {
    m_view.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setWidget(&m_view);
    setWidgetResizable(true);
    m_view.installEventFilter(this);
  }

  void logMessage(quint64 pid, qint64 time, const QByteArray &msg)
  {
    m_view.m_data.append({ time, pid, msg });
    m_view.updateSize();
  }

  void setLoggingClient(quint64 pid)
  {
    m_view.m_client = pid;
    m_view.update();
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
  return {200, 200};
}

void LogView::logMessage(quint64 pid, qint64 time, const QByteArray &msg)
{
  m_messages->logMessage(pid, time, msg);
  m_timeline->logMessage(pid, time, msg);
}

void LogView::setLoggingClient(quint64 pid)
{
  m_messages->setLoggingClient(pid);
  m_timeline->setLoggingClient(pid);
}

void LogView::reset()
{
  m_messages->reset();
}

}
