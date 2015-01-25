/*
  signalhistorydelegate.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SIGNALHISTORYDELEGATE_H
#define GAMMARAY_SIGNALHISTORYDELEGATE_H

#include <QStyledItemDelegate>

namespace GammaRay {

class SignalHistoryDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  Q_PROPERTY(qint64 visibleInterval READ visibleInterval WRITE setVisibleInterval NOTIFY visibleIntervalChanged)
  Q_PROPERTY(qint64 visibleOffset READ visibleOffset NOTIFY setVisibleOffset NOTIFY visibleOffsetChanged)
  Q_PROPERTY(bool isActive READ isActive WRITE setActive NOTIFY isActiveChanged)

  public:
    explicit SignalHistoryDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setVisibleInterval(qint64 interval);
    qint64 visibleInterval() const { return m_visibleInterval; }

    void setVisibleOffset(qint64 offset);
    qint64 visibleOffset() const { return m_visibleOffset; }

    qint64 totalInterval() const { return m_totalInterval; }

    void setActive(bool isActive);
    bool isActive() const;

    QString toolTipAt(const QModelIndex &index, int position, int width);

  signals:
    void visibleIntervalChanged(qint64 value);
    void visibleOffsetChanged(qint64 value);
    void isActiveChanged(bool value);
    void totalIntervalChanged();

  private slots:
    void onUpdateTimeout();
    void onServerClockChanged(qlonglong msecs);

  private:
    QTimer *const m_updateTimer;
    qint64 m_visibleOffset;
    qint64 m_visibleInterval;
    qint64 m_totalInterval;
};

} // namespace GammaRay

#endif // GAMMARAY_SIGNALHISTORYDELEGATE_H
