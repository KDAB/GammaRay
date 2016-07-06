/*
  qt3dinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_QT3DINSPECTOR_QT3DINSPECTORWIDGET_H
#define GAMMARAY_QT3DINSPECTOR_QT3DINSPECTORWIDGET_H

#include <ui/tooluifactory.h>
#include <ui/uistatemanager.h>

#include <QWidget>

#include <memory>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class Qt3DInspectorInterface;

namespace Ui {
class Qt3DInspectorWidget;
}

class Qt3DInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Qt3DInspectorWidget(QWidget *parent = nullptr);
    ~Qt3DInspectorWidget();

private:
    void entityContextMenu(QPoint pos);
    void frameGraphContextMenu(QPoint pos);

    void entitySelectionChanged(const QItemSelection &selection, const QItemSelection &deselected);
    void frameGraphSelectionChanged(const QItemSelection &selection, const QItemSelection &deselected);

    std::unique_ptr<Ui::Qt3DInspectorWidget> ui;
    UIStateManager m_stateManager;
    Qt3DInspectorInterface *m_interface;

private slots:
    void propertyWidgetTabsChanged();
};

class Qt3DInspectorUiFactory : public QObject, public StandardToolUiFactory<Qt3DInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_3dinspector.json")

public:
    void initUi() override;
};
}

#endif
