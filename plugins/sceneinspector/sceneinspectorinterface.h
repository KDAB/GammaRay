/*
  sceneinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SCENEINSPECTOR_SCENEINSPECTORINTERFACE_H
#define GAMMARAY_SCENEINSPECTOR_SCENEINSPECTORINTERFACE_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QPainter;
class QGraphicsItem;
class QSize;
class QTransform;
class QRectF;
class QPixmap;
class QPointF;
QT_END_NAMESPACE

namespace GammaRay {
class SceneInspectorInterface : public QObject
{
    Q_OBJECT
public:
    explicit SceneInspectorInterface(QObject *parent = nullptr);
    ~SceneInspectorInterface() override;

    virtual void initializeGui() = 0;

    static void paintItemDecoration(QGraphicsItem *item, const QTransform &transform,
                                    QPainter *painter);

public slots:
    virtual void renderScene(const QTransform &transform, const QSize &size) = 0;
    virtual void sceneClicked(const QPointF &pos) = 0;

signals:
    void sceneRectChanged(const QRectF &rect);
    void sceneChanged();
    void sceneRendered(const QPixmap &view);
    void itemSelected(const QRectF &boundingRect);
};
}

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::SceneInspectorInterface, "com.kdab.GammaRay.SceneInspector")
QT_END_NAMESPACE

#endif // GAMMARAY_SCENEINSPECTORINTERFACE_H
