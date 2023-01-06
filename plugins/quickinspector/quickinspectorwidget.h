/*
  quickinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    enum StateFlag
    {
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
