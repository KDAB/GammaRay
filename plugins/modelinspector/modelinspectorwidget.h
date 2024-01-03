/*
  modelinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_MODELINSPECTOR_MODELINSPECTORWIDGET_H
#define GAMMARAY_MODELINSPECTOR_MODELINSPECTORWIDGET_H

#include <ui/tooluifactory.h>
#include <ui/uistatemanager.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QItemSelection;
class QModelIndex;
QT_END_NAMESPACE

namespace GammaRay {
class ModelInspectorInterface;

namespace Ui {
class ModelInspectorWidget;
}

class ModelInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ModelInspectorWidget(QWidget *parent = nullptr);
    ~ModelInspectorWidget() override;

private slots:
    void cellDataChanged();
    void objectRegistered(const QString &objectName);
    void modelSelected(const QItemSelection &selected);
    void modelContextMenu(QPoint pos);
    void selectionModelContextMenu(QPoint pos);

private:
    QScopedPointer<Ui::ModelInspectorWidget> ui;
    UIStateManager m_stateManager;
    ModelInspectorInterface *m_interface;
};

class ModelInspectorUiFactory : public QObject, public StandardToolUiFactory<ModelInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_modelinspector.json")
};
}

#endif // GAMMARAY_MODELINSPECTORWIDGET_H
