/*
  signalhistoryview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
