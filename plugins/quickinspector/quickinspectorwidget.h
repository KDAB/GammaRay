/*
  quickinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORWIDGET_H
#define GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>
#include <ui/propertywidget.h>
#include <common/objectbroker.h>
#include "quickinspectorinterface.h"

#include <QWidget>
#include <QVariant>

QT_BEGIN_NAMESPACE
class QLabel;
class QImage;
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class QuickSceneControlWidget;
struct QuickDecorationsSettings;

namespace Ui {
class QuickInspectorWidget;
}

class QuickInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    enum StateFlag {
        Ready = 0x0,
        WaitingApply = 0x1,
        WaitingFeatures = 0x2,
        WaitingOverlaySettings = 0x8,
        WaitingAll = WaitingApply | WaitingFeatures | WaitingOverlaySettings
    };
    Q_DECLARE_FLAGS(State, StateFlag)

    explicit QuickInspectorWidget(QWidget *parent = nullptr);
    ~QuickInspectorWidget() override;

    Q_INVOKABLE void saveTargetState(QSettings *settings) const;
    Q_INVOKABLE void restoreTargetState(QSettings *settings);

private slots:
    void itemSelectionChanged(const QItemSelection &selection);
    void sgSelectionChanged(const QItemSelection &selection);
    void setFeatures(GammaRay::QuickInspectorInterface::Features features);
    void setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings);
    void setSlowMode(bool slow);
    void itemModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                              const QVector<int> &roles);
    void itemContextMenu(const QPoint &pos);
    void stateReceived(GammaRay::QuickInspectorWidget::StateFlag flag);
    void resetState();
    void saveState();
    void saveAsImage();
    void updateActions();

private:
    QScopedPointer<Ui::QuickInspectorWidget> ui;
    QuickInspectorWidget::State m_state;
    UIStateManager m_stateManager;
    QuickSceneControlWidget *m_scenePreviewWidget;
    QuickInspectorInterface *m_interface;
};

class QuickInspectorUiFactory : public QObject, public StandardToolUiFactory<QuickInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_quickinspector.json")

    void initUi() override;
};
}

Q_DECLARE_METATYPE(GammaRay::QuickInspectorWidget::StateFlag)

#endif
