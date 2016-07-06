/*
  sceneinspectorwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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
    explicit SceneInspectorWidget(QWidget *parent = 0);
    ~SceneInspectorWidget();

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
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

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
    void initUi() Q_DECL_OVERRIDE;
};
}

#endif // GAMMARAY_SCENEINSPECTOR_H
