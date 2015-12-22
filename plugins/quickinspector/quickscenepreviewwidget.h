/*
  quickscenepreviewwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKSCENEPREVIEWWIDGET_H
#define GAMMARAY_QUICKINSPECTOR_QUICKSCENEPREVIEWWIDGET_H

#include "quickitemgeometry.h"
#include "quickinspectorinterface.h"

#include <QtWidgets/QWidget>

class QAction;
class QActionGroup;
class QComboBox;
class QLabel;
class QToolBar;

namespace GammaRay {

class QuickInspectorInterface;

class QuickScenePreviewWidget : public QWidget
{
  Q_OBJECT

  enum MouseMode {
      MovePreview,
      MeasurePixels,
      RedirectInput
  };

  public:
    explicit QuickScenePreviewWidget(QuickInspectorInterface *inspector, QWidget *parent = 0);
    ~QuickScenePreviewWidget();

    void setItemGeometry(const QuickItemGeometry &itemGeometry);
    void setImage(const QImage &image);

    void setSupportsCustomRenderModes(QuickInspectorInterface::Features  supportedCustomRenderModes);

    void setZoom(qreal zoom);

  private Q_SLOTS:
    void setZoomFromCombobox(int index);
    void visualizeActionTriggered(bool checked);
    void setMouseTool(QAction *action);

  private:
    void drawGeometry(QPainter *p);
    void drawRuler(QPainter *p);
    void drawArrow(QPainter *p, QPointF first, QPointF second);
    void drawAnchor(QPainter *p, Qt::Orientation orientation,
                    qreal ownAnchorLine, qreal offset, const QString &label);
    void drawMeasureLine(QPainter *p);
    void updateEffectiveGeometry();

  private:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;

    struct {
        QToolBar *toolbarWidget;
        QComboBox *zoomCombobox;
        QActionGroup *visualizeGroup;
        QAction *visualizeClipping;
        QAction *visualizeOverdraw;
        QAction *visualizeBatches;
        QAction *visualizeChanges;

        QAction *movePreview;
        QAction *measurePixels;
        QAction *redirectInput;

        QLabel *measureLabel;
    } m_toolBar;

    QuickInspectorInterface *m_inspectorInterface;
    MouseMode m_mouseMode;

    qreal m_zoom;
    int m_x;
    int m_y;
    QImage m_image;

    QPoint m_mouseGrabPosition; // position where the mouse was pressed down, relative to image but in widget scale
    QPoint m_zoomedMousePosition; // current mouse position, in scene coordinates
    QPoint m_zoomedMouseGrabPosition; // position in scene coordinates where the mouse was pressed down
    bool m_mousePressed;

    QuickItemGeometry m_itemGeometry;
    QuickItemGeometry m_effectiveGeometry; // scaled and translated

    QBrush m_backgroundBrush;
};

} // namespace GammaRay

#endif
