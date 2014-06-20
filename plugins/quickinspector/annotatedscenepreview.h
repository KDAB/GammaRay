/*
  quickitemoverlay.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#ifndef GAMMARAY_QUICKINSPECTOR_ANNOTATEDSCENEPREVIEW_H
#define GAMMARAY_QUICKINSPECTOR_ANNOTATEDSCENEPREVIEW_H

#include <QQuickPaintedItem>
#include <QTransform>
#include <QImage>

namespace GammaRay {

class AnnotatedScenePreview : public QQuickPaintedItem
{
  Q_OBJECT
  Q_PROPERTY(QVariantMap previewData READ previewData WRITE setPreviewData NOTIFY previewDataChanged)
  Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
  Q_PROPERTY(QSize sourceSize READ sourceSize NOTIFY sourceSizeChanged)
  Q_PROPERTY(QSize margin READ margin WRITE setMargin NOTIFY marginChanged)

  public:
    explicit AnnotatedScenePreview(QQuickItem *parent = 0);
    virtual ~AnnotatedScenePreview();
    virtual void paint(QPainter *p);

    QVariantMap previewData() const;
    qreal zoom() const;
    QSize sourceSize() const;

    QSize margin() const;
    void setMargin(QSize margin);

  Q_SIGNALS:
    void zoomChanged();
    void sourceSizeChanged();
    void previewDataChanged();
    void marginChanged();

  public Q_SLOTS:
    void setPreviewData(QVariantMap previewData);
    void setZoom(qreal zoom);

  private:
    void drawArrow(QPainter *p, QPointF first, QPointF second);
    void drawAnchor(QPainter *p, Qt::Orientation orientation,
                    qreal ownAnchorLine, qreal offset, const QString &label);
    void updatePreviewData();

  private:
    QVariantMap m_previewData;
    QImage m_image;
    QSize m_margin;
    qreal m_zoom;
    QRectF m_itemRect;
    QRectF m_boundingRect;
    QRectF m_childrenRect;
    QPointF m_transformOriginPoint;
    QTransform m_transform;
    QTransform m_parentTransform;
    qreal m_leftMargin;
    qreal m_horizonalCenterOffset;
    qreal m_rightMargin;
    qreal m_topMargin;
    qreal m_verticalCenterOffset;
    qreal m_bottomMargin;
    qreal m_baselineOffset;
    qreal m_x;
    qreal m_y;
};

}

#endif // ANNOTATEDSCENEPREVIEW_H
