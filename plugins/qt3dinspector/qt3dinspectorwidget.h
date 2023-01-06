/*
  qt3dinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
