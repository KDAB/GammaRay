/*
  signalhistoryview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SIGNALHISTORYVIEW_H
#define GAMMARAY_SIGNALHISTORYVIEW_H

#include "ui/deferredtreeview.h"

namespace GammaRay {
class SignalHistoryDelegate;

class SignalHistoryView : public DeferredTreeView
{
    Q_OBJECT
    Q_PROPERTY(
        QScrollBar *eventScrollBar READ eventScrollBar WRITE setEventScrollBar NOTIFY eventScrollBarChanged)
    Q_PROPERTY(SignalHistoryDelegate *eventDelegate READ eventDelegate FINAL CONSTANT)

public:
    explicit SignalHistoryView(QWidget *parent = nullptr);

    void setEventScrollBar(QScrollBar *scrollBar);
    QScrollBar *eventScrollBar() const
    {
        return m_eventScrollBar;
    }
    SignalHistoryDelegate *eventDelegate() const
    {
        return m_eventDelegate;
    }

    int eventColumnPosition() const;
    int eventColumnWidth() const;

signals:
    void eventScrollBarChanged(QScrollBar *scrollBar);

protected:
    bool viewportEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *) override;

private slots:
    void eventDelegateChanged();
    void eventScrollBarSliderMoved(int value);

private:
    SignalHistoryDelegate *const m_eventDelegate;
    QScrollBar *m_eventScrollBar;
};
} // namespace GammaRay

#endif // GAMMARAY_SIGNALHISTORYVIEW_H
