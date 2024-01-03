/*
  graphicssceneview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SCENEINSPECTOR_GRAPHICSSCENEVIEW_H
#define GAMMARAY_SCENEINSPECTOR_GRAPHICSSCENEVIEW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsItem;
QT_END_NAMESPACE

namespace GammaRay {
class GraphicsView;

namespace Ui {
class GraphicsSceneView;
}

class GraphicsSceneView : public QWidget
{
    Q_OBJECT
public:
    explicit GraphicsSceneView(QWidget *parent = nullptr);
    ~GraphicsSceneView() override;

    GraphicsView *view() const;

    void showGraphicsItem(QGraphicsItem *item);
    void setGraphicsScene(QGraphicsScene *scene);

private slots:
    void sceneCoordinatesChanged(const QPointF &coord);
    void itemCoordinatesChanged(const QPointF &coord);

private:
    Ui::GraphicsSceneView *ui;
};
}

#endif // GAMMARAY_GRAPHICSSCENEVIEW_H
