/*
  sceneinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SCENEINSPECTOR_SCENEINSPECTORWIDGET_H
#define GAMMARAY_SCENEINSPECTOR_SCENEINSPECTORWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QGraphicsPixmapItem;
class QGraphicsScene;
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class SceneInspectorInterface;

namespace Ui {
class SceneInspectorWidget;
}

class SceneInspectorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SceneInspectorWidget(QWidget *parent = nullptr);
    ~SceneInspectorWidget() override;

private slots:
    void sceneSelected(int index);
    void sceneItemSelected(const QItemSelection &selection);
    void sceneRectChanged(const QRectF &rect);
    void sceneChanged();
    void requestSceneUpdate();
    void sceneRendered(const QPixmap &view);
    void visibleSceneRectChanged();
    void itemSelected(const QRectF &boundingRect);
    void sceneContextMenu(QPoint pos);
    void propertyWidgetTabsChanged();

private:
    bool eventFilter(QObject *obj, QEvent *event) override;

    QScopedPointer<Ui::SceneInspectorWidget> ui;
    UIStateManager m_stateManager;
    SceneInspectorInterface *m_interface;
    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_pixmap;
    QTimer *m_updateTimer;
};

class SceneInspectorUiFactory : public QObject, public StandardToolUiFactory<SceneInspectorWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_sceneinspector.json")
public:
    void initUi() override;
};
}

#endif // GAMMARAY_SCENEINSPECTOR_H
