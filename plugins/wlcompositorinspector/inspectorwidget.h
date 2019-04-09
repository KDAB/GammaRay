/*
  inspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

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

#ifndef GAMMARAY_WLCOMPOSITORINSPECTOR_WIDGET_H
#define GAMMARAY_WLCOMPOSITORINSPECTOR_WIDGET_H

#include <ui/tooluifactory.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {

namespace Ui {
  class InspectorWidget;
}

class WlCompositorInterface;
class LogView;

class InspectorWidget : public QWidget
{
Q_OBJECT
public:
    explicit InspectorWidget(QWidget *parent = nullptr);
    ~InspectorWidget() override;

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void delayedInit();

private:
    void clientSelected(const QItemSelection &selection);
    void clientContextMenu(QPoint pos);
    void resourceActivated(const QModelIndex &index);
    void logMessage(const QByteArray &msg);

    QScopedPointer<Ui::InspectorWidget> m_ui;
    QAbstractItemModel *m_model;
    WlCompositorInterface *m_client;
    LogView *m_logView;
};

class InspectorWidgetFactory : public QObject, public StandardToolUiFactory<InspectorWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolUiFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_wlcompositorinspector.json")
};


}

#endif // GAMMARAY_FONTBROWSERWIDGET_H
