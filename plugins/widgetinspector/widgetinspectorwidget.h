/*
  widgetinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORWIDGET_H
#define GAMMARAY_WIDGETINSPECTOR_WIDGETINSPECTORWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class WidgetInspectorInterface;
class Widget3DView;
class WidgetRemoteView;

namespace Ui {
class WidgetInspectorWidget;
}

class WidgetInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetInspectorWidget(QWidget *parent = nullptr);
    ~WidgetInspectorWidget() override;

    Q_INVOKABLE void saveTargetState(QSettings *settings) const;
    Q_INVOKABLE void restoreTargetState(QSettings *settings);

private slots:
    void widgetSelected(const QItemSelection &selection);
    void widgetTreeContextMenu(QPoint pos);

    void saveAsImage();
    void saveAsSvg();
    void saveAsUiFile();
    void analyzePainting();
    void updateActions();
    void propertyWidgetTabsChanged();

private:
    QScopedPointer<Ui::WidgetInspectorWidget> ui;
    UIStateManager m_stateManager;
    WidgetInspectorInterface *m_inspector;
    WidgetRemoteView *m_remoteView;
    Widget3DView *m_3dView;
};

class WidgetInspectorUiFactory : public QObject, public StandardToolUiFactory<WidgetInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_widgetinspector.json")
public:
    void initUi() override;
};
}

#endif // GAMMARAY_WIDGETINSPECTOR_H
