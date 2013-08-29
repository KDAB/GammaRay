/*
  signalmonitorwidget.h

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

#ifndef GAMMARAY_SIGNALMONITORWIDGET_H
#define GAMMARAY_SIGNALMONITORWIDGET_H

#include <QWidget>

namespace GammaRay {

namespace Ui {
  class SignalMonitorWidget;
}

class SignalHistoryDelegate;

class SignalMonitorWidget : public QWidget
{
  Q_OBJECT
  public:
    explicit SignalMonitorWidget(QWidget *parent = 0);
    ~SignalMonitorWidget();

  protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;

  private slots:
    void adjustEventScale(int value);
    void adjustEventRange();
    void adjustEventOffset(int value);
    void adjustEventScrollBarSize();
    void pauseAndResume(bool pause);
    void eventsActiveChanged(bool active);

    int eventColumnPosition() const;
    int eventColumnWidth() const;

  private:
    QScopedPointer<Ui::SignalMonitorWidget> ui;
    SignalHistoryDelegate *const m_eventDelegate;
};

} // namespace GammaRay

#endif // GAMMARAY_SIGNALMONITORWIDGET_H
