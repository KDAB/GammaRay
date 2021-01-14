/*
  signalhistorydelegate.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#include "signalhistorydelegate.h"
#include "signalhistorymodel.h"
#include "signalmonitorinterface.h"
#include "signalmonitorcommon.h"

#include <common/metatypedeclarations.h>
#include <common/objectbroker.h>

#include <QDebug>
#include <QPainter>
#include <QTimer>

#include <limits>

using namespace GammaRay;

SignalHistoryDelegate::SignalHistoryDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_updateTimer(new QTimer(this))
    , m_visibleOffset(0)
    , m_visibleInterval(15000)
    , m_totalInterval(0)
{
    connect(m_updateTimer, &QTimer::timeout, this, &SignalHistoryDelegate::onUpdateTimeout);
    m_updateTimer->start(1000 / 25);
    onUpdateTimeout();

    SignalMonitorInterface *iface = ObjectBroker::object<SignalMonitorInterface *>();
    connect(iface, &SignalMonitorInterface::clock, this, &SignalHistoryDelegate::onServerClockChanged);
    iface->sendClockUpdates(true);
}

void SignalHistoryDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    const qint64 interval = m_visibleInterval;
    const qint64 startTime = m_visibleOffset;
    const qint64 endTime = startTime + interval;

    const QAbstractItemModel * const model = index.model();
    const QVector<qint64> &events
        = model->data(index, SignalHistoryModel::EventsRole).value<QVector<qint64> >();
    const qint64 t0
        = qMax(static_cast<qint64>(0),
               model->data(index, SignalHistoryModel::StartTimeRole).value<qint64>() - startTime);
    qint64 t1 = model->data(index, SignalHistoryModel::EndTimeRole).value<qint64>();
    if (t1 < 0) // still alive
        t1 = m_totalInterval;
    t1 -= startTime;
    const qint64 dt = qMax(0LL, t1) - t0;

    const int x0 = option.rect.x() + 1;
    const int y0 = option.rect.y();
    const int dx = option.rect.width() - 2;
    const int dy = option.rect.height();
    const int x1 = x0 + dx * t0 / interval;
    const int x2 = dx * dt / interval + 1;

    if (t1 >= 0)
        painter->fillRect(x1, y0 + 1, x2, dy - 2, option.palette.window());

    painter->setPen(option.palette.color(QPalette::WindowText));

    for (qint64 ev : events) {
        const qint64 ts = SignalHistoryModel::timestamp(ev);
        if (ts >= startTime && ts < endTime) {
            const int x = x0 + dx * (ts - startTime) / interval;
            painter->drawLine(x, y0 + 1, x, y0 + dy - 2);
        }
    }
}

QSize SignalHistoryDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &) const
{
    return {0, option.fontMetrics.lineSpacing()}; // FIXME: minimum height
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
    // move the visible region to show the most recent samples
    m_visibleOffset = m_totalInterval - m_visibleInterval;
    emit visibleOffsetChanged(m_visibleOffset);
}

void SignalHistoryDelegate::onServerClockChanged(qint64 msecs)
{
    m_totalInterval = msecs;
    emit totalIntervalChanged();
}

void SignalHistoryDelegate::setActive(bool active)
{
    if (m_updateTimer->isActive() != active) {
        if (active)
            m_updateTimer->start();
        else
            m_updateTimer->stop();

        emit isActiveChanged(isActive());
    }
}

bool SignalHistoryDelegate::isActive() const
{
    return m_updateTimer->isActive();
}

QString SignalHistoryDelegate::toolTipAt(const QModelIndex &index, int position, int width)
{
    const QAbstractItemModel * const model = index.model();
    const QVector<qint64> &events
        = model->data(index, SignalHistoryModel::EventsRole).value<QVector<qint64> >();

    const qint64 t = m_visibleInterval * position / width + m_visibleOffset;
    qint64 dtMin = std::numeric_limits<qint64>::max();
    int signalIndex = -1;
    qint64 signalTimestamp = -1;

    for (long long event : events) {
        signalTimestamp = SignalHistoryModel::timestamp(event);
        const qint64 dt = qAbs(signalTimestamp - t);

        if (dt < dtMin) {
            signalIndex = SignalHistoryModel::signalIndex(event);
            dtMin = dt;
        }
    }

    if (signalIndex < 0)
        return QString();

    const auto signalNames
        = index.data(SignalHistoryModel::SignalMapRole).value<QHash<int, QByteArray> >();
    const auto it = signalNames.constFind(signalIndex);
    QString signalName;
    // see SignalHistoryModel, we store this with offset 1 to fit unknown ones into an unsigned value
    if (signalIndex == 0 || it == signalNames.constEnd() || it.value().isEmpty())
        signalName = tr("<unknown>");
    else
        signalName = it.value();

    const QString &ts = QLocale().toString(signalTimestamp);
    return tr("%1 at %2 ms").arg(signalName, ts);
}
