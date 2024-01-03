/*
  graphicsview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SCENEINSPECTOR_GRAPHICSVIEW_H
#define GAMMARAY_SCENEINSPECTOR_GRAPHICSVIEW_H

#include <QGraphicsView>

namespace GammaRay {
class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = nullptr);

    void showItem(QGraphicsItem *item);

signals:
    void sceneCoordinatesChanged(const QPointF &sceneCoord);
    void itemCoordinatesChanged(const QPointF &itemCoord);
    void transformChanged();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void drawForeground(QPainter *painter, const QRectF &rect) override;

private:
    QGraphicsItem *m_currentItem;
};
}

#endif // GAMMARAY_GRAPHICSVIEW_H
