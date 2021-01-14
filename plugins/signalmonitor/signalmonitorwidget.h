/*
  signalmonitorwidget.h

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

#ifndef GAMMARAY_SIGNALMONITORWIDGET_H
#define GAMMARAY_SIGNALMONITORWIDGET_H

#include <ui/uistatemanager.h>
#include "ui/tooluifactory.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
namespace Ui {
class SignalMonitorWidget;
}

class SignalMonitorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SignalMonitorWidget(QWidget *parent = nullptr);
    ~SignalMonitorWidget() override;

private slots:
    void intervalScaleValueChanged(int value);
    void adjustEventScrollBarSize();
    void pauseAndResume(bool pause);
    void eventDelegateIsActiveChanged(bool active);
    void contextMenu(QPoint pos);
    void selectionChanged(const QItemSelection &selection);

private:
    static const QString ITEM_TYPE_NAME_OBJECT;
    QScopedPointer<Ui::SignalMonitorWidget> ui;
    UIStateManager m_stateManager;
};

class SignalMonitorUiFactory : public QObject, public StandardToolUiFactory<SignalMonitorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_signalmonitor.json")
};
} // namespace GammaRay

#endif // GAMMARAY_SIGNALMONITORWIDGET_H
