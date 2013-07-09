/*
  signalhistorydelegate.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#include "signalhistorydelegate.h"

#include <core/tools/signalmonitor/signalhistorymodel.h>

#include <QDebug>
#include <QDateTime>
#include <QPainter>
#include <QTimer>

using namespace GammaRay;

SignalHistoryDelegate::SignalHistoryDelegate(QObject *parent)
  : QAbstractItemDelegate(parent)
  , m_updateTimer(new QTimer(this))
  , m_beginOfTime(QDateTime::currentMSecsSinceEpoch()) // FIXME: rather get a timestamp from app start
  , m_endOfTime(m_visibleOffset)
  , m_visibleInterval(15000)
  , m_visibleOffset(0)
{
  connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTimeout()));
  m_updateTimer->start(1000 / 25);
  onUpdateTimeout();
}

void SignalHistoryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  const qint64 interval = m_visibleInterval;
  const qint64 startTime = m_visibleOffset;
  const qint64 endTime = startTime + interval;

  const QAbstractItemModel *const model = index.model();
  const QVector<qint64> &events = model->data(index, SignalHistoryModel::EventsRole).value<QVector<qint64> >();
  const qint64 t0 = qMax(0LL, model->data(index, SignalHistoryModel::StartTimeRole).value<qint64>() - startTime);
  const qint64 t1 = model->data(index, SignalHistoryModel::EndTimeRole).value<qint64>() - startTime;
  const qint64 dt = qMax(0LL, t1) - t0;

  const int x0 = option.rect.x() + 1;
  const int y0 = option.rect.y();
  const int dx = option.rect.width() - 2;
  const int dy = option.rect.height();
  const int x1 = x0 + dx * t0 / interval;
  const int x2 = dx * dt / interval + 1;

  const bool selected = option.state & QStyle::State_Selected;

  if (selected) {
    painter->fillRect(option.rect, option.palette.highlight());
  }

  if (t1 >= 0) {
    painter->fillRect(x1, y0 + 1, x2, dy - 2,
                      selected ? option.palette.highlightedText()
                               : option.palette.midlight());
  }

  painter->setPen(option.palette.color(selected ? QPalette::Highlight : QPalette::WindowText));

  foreach (qint64 ev, events) {
    const qint64 ts = SignalHistoryModel::timestamp(ev);
    if (ts >= startTime && ts < endTime) {
      const int x = x0 + dx * (ts - startTime) / interval;
      painter->drawLine(x, y0, x, y0 + dy);
    }
  }
}

QSize SignalHistoryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
  return QSize(0, option.fontMetrics.lineSpacing()); // FIXME: minimum height
}

void SignalHistoryDelegate::setVisibleInterval(qint64 interval)
{
  if (m_visibleInterval != interval) {
    m_visibleInterval = interval;
    emit visibleIntervalChanged(m_visibleInterval);
  }
}

void SignalHistoryDelegate::setVisibleOffset(qint64 offset)
{
  setActive(false);

  if (m_visibleOffset != offset) {
    m_visibleOffset = offset;
    emit visibleOffsetChanged(m_visibleOffset);
  }
}

void SignalHistoryDelegate::onUpdateTimeout()
{
  const qint64 t = QDateTime::currentMSecsSinceEpoch();
  m_endOfTime = (t - (t % m_updateTimer->interval())); // align to fixed intervals
  m_visibleOffset = m_endOfTime - m_visibleInterval;
  emit visibleOffsetChanged(m_visibleOffset);
}

void SignalHistoryDelegate::setActive(bool active)
{
  if (m_updateTimer->isActive() != active) {
    if (active) {
      m_updateTimer->start();
    } else {
      m_updateTimer->stop();
    }

    emit isActiveChanged(isActive());
  }
}

bool SignalHistoryDelegate::isActive() const
{
  return m_updateTimer->isActive();
}

