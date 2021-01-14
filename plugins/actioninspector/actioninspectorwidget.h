/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#ifndef GAMMARAY_ACTIONINSPECTOR_ACTIONINSPECTORWIDGET_H
#define GAMMARAY_ACTIONINSPECTOR_ACTIONINSPECTORWIDGET_H

#include <QAction>
#include <QWidget>

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {

namespace Ui {
class ActionInspectorWidget;
}

class ActionInspectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActionInspectorWidget(QWidget *parent = nullptr);
    ~ActionInspectorWidget() override;

private Q_SLOTS:
    void triggerAction(const QModelIndex &index);
    void contextMenu(QPoint pos);
    void selectionChanged(const QItemSelection &selection);

private:
    QScopedPointer<Ui::ActionInspectorWidget> ui;
    UIStateManager m_stateManager;
};

class ActionInspectorUiFactory : public QObject, public StandardToolUiFactory<ActionInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_actioninspector.json")
};
}

#endif // GAMMARAY_ACTIONINSPECTORWIDGET_H
