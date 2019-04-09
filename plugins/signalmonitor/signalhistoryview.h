/*
  signalhistoryview.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    QScrollBar *eventScrollBar() const { return m_eventScrollBar; }
    SignalHistoryDelegate *eventDelegate() const { return m_eventDelegate; }

    int eventColumnPosition() const;
    int eventColumnWidth() const;

signals:
    void eventScrollBarChanged(QScrollBar *scrollBar);

protected:
    bool viewportEvent(QEvent *event) override;

private slots:
    void eventDelegateChanged();
    void eventScrollBarSliderMoved(int value);

private:
    SignalHistoryDelegate * const m_eventDelegate;
    QScrollBar *m_eventScrollBar;
};
} // namespace GammaRay

#endif // GAMMARAY_SIGNALHISTORYVIEW_H
