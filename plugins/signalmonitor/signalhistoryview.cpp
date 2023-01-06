/*
  signalhistoryview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "signalhistoryview.h"
#include "signalhistorydelegate.h"
#include "signalhistorymodel.h"
#include "signalmonitorwidget.h"

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
        const QHelpEvent *const help = static_cast<QHelpEvent *>(event);
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

void SignalHistoryView::wheelEvent(QWheelEvent *e)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    auto pos = e->pos();
#else
    auto pos = e->position().toPoint();
#endif
    if (pos.x() < eventColumnPosition()) {
        DeferredTreeView::wheelEvent(e);
        return;
    }

    if (e->modifiers() & Qt::ControlModifier) {
        const qint64 interval = m_eventDelegate->intervalForPosition(pos.x() - eventColumnPosition(), eventColumnWidth());
        if (interval <= 0) {
            DeferredTreeView::wheelEvent(e);
            return;
        }
        int y = e->angleDelta().y();
        auto widget = static_cast<SignalMonitorWidget *>(parentWidget());
        widget->zoomSlider()->setValue(widget->zoomSlider()->value() + y / 16);
        m_eventDelegate->setVisibleOffset(interval);

        qint64 newInterval = m_eventDelegate->intervalForPosition(pos.x() - eventColumnPosition(), eventColumnWidth());
        if (newInterval > interval) {
            auto diff = newInterval - interval;
            auto newPos = interval - diff;
            m_eventDelegate->setVisibleOffset(newPos);
        }
        return;
    }
    DeferredTreeView::wheelEvent(e);
}

#include "moc_signalhistoryview.cpp"
