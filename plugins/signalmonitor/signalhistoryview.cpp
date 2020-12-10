/*
  signalhistoryview.cpp

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

#include "signalhistoryview.h"
#include "signalhistorydelegate.h"
#include "signalhistorymodel.h"

#include <QHelpEvent>
#include <QScrollBar>
#include <QToolTip>

using namespace GammaRay;

SignalHistoryView::SignalHistoryView(QWidget *parent)
    : DeferredTreeView(parent)
    , m_eventDelegate(new SignalHistoryDelegate(this))
    , m_eventScrollBar(nullptr)
{
    setDeferredResizeMode(0, QHeaderView::Interactive);
    setDeferredResizeMode(1, QHeaderView::Interactive);
    setDeferredResizeMode(2, QHeaderView::Stretch);

    setItemDelegateForColumn(SignalHistoryModel::EventColumn, m_eventDelegate);

    connect(m_eventDelegate, &SignalHistoryDelegate::visibleOffsetChanged, this, &SignalHistoryView::eventDelegateChanged);
    connect(m_eventDelegate, &SignalHistoryDelegate::visibleIntervalChanged, this,
            &SignalHistoryView::eventDelegateChanged);
    connect(m_eventDelegate, &SignalHistoryDelegate::totalIntervalChanged, this, &SignalHistoryView::eventDelegateChanged);
}

void SignalHistoryView::eventDelegateChanged()
{
    viewport()->update(eventColumnPosition(), 0, eventColumnWidth(), height());

    if (m_eventScrollBar) {
        const bool signalsBlocked = m_eventScrollBar->blockSignals(true);

        // With 31 bits we cover more than 24 days when counting milliseconds.
        // That's much more time than this tool can handle. IMHO.
        m_eventScrollBar->setMaximum(
            qMax(m_eventDelegate->totalInterval() - m_eventDelegate->visibleInterval(), 0LL));

        m_eventScrollBar->setSingleStep(m_eventDelegate->visibleInterval() / 10);
        m_eventScrollBar->setPageStep(m_eventDelegate->visibleInterval());

        if (m_eventDelegate->isActive())
            m_eventScrollBar->setValue(m_eventScrollBar->maximum());

        m_eventScrollBar->blockSignals(signalsBlocked);
    }
}

void SignalHistoryView::setEventScrollBar(QScrollBar *scrollBar)
{
    if (m_eventScrollBar != scrollBar) {
        if (m_eventScrollBar)
            disconnect(m_eventScrollBar, nullptr, this, nullptr);

        m_eventScrollBar = scrollBar;

        if (m_eventScrollBar) {
            connect(m_eventScrollBar, &QAbstractSlider::sliderMoved,
                    this, &SignalHistoryView::eventScrollBarSliderMoved);
        }
    }
}

int SignalHistoryView::eventColumnPosition() const
{
    return columnViewportPosition(SignalHistoryModel::EventColumn);
}

int SignalHistoryView::eventColumnWidth() const
{
    return columnWidth(SignalHistoryModel::EventColumn);
}

void SignalHistoryView::eventScrollBarSliderMoved(int value)
{
    m_eventDelegate->setActive(false);
    m_eventDelegate->setVisibleOffset(value);
}

bool SignalHistoryView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        const QHelpEvent * const help = static_cast<QHelpEvent *>(event);
        const QModelIndex index = indexAt(help->pos());

        if (index.isValid() && index.column() == SignalHistoryModel::EventColumn) {
            const int x0 = help->pos().x() - eventColumnPosition();
            const int dx = eventColumnWidth();
            const QString &toolTipText = m_eventDelegate->toolTipAt(index, x0, dx);

            if (!toolTipText.isEmpty()) {
                QToolTip::showText(help->globalPos(), toolTipText);
            } else {
                QToolTip::hideText();
                event->ignore();
            }

            return true;
        }
    }

    return DeferredTreeView::viewportEvent(event);
}

#include "moc_signalhistoryview.cpp"
