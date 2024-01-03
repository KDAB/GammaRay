/*
  actioninspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    static void triggerAction(const QModelIndex &index);
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
