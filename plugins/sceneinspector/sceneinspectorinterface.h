/*
  sceneinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
