/*
  quickdecorationsdrawer.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKDECORATIONSDRAWER_H
#define GAMMARAY_QUICKINSPECTOR_QUICKDECORATIONSDRAWER_H

#include <QPen>

#include "quickitemgeometry.h"

namespace GammaRay {

struct QuickDecorationsSettings
{
    QuickDecorationsSettings()
        : boundingRectColor(QColor(232, 87, 82, 170))
        , boundingRectBrush(QBrush(QColor(232, 87, 82, 95)))
        , geometryRectColor(QColor(Qt::gray))
        , geometryRectBrush(QBrush(QColor(Qt::gray), Qt::BDiagPattern))
        , childrenRectColor(QColor(0, 99, 193, 170))
        , childrenRectBrush(QBrush(QColor(0, 99, 193, 95)))
        , transformOriginColor(QColor(156, 15, 86, 170))
        , coordinatesColor(QColor(136, 136, 136))
        , marginsColor(QColor(139, 179, 0))
        , paddingColor(QColor(Qt::darkBlue))
        , gridOffset(QPointF(0, 0))
        , gridCellSize(QSizeF(0, 0))
        , gridColor(QColor(Qt::red))
        , componentsTraces(false)
        , gridEnabled(false)
    {
    }

    bool operator==(const QuickDecorationsSettings &other) const {
        return boundingRectColor == other.boundingRectColor &&
                boundingRectBrush == other.boundingRectBrush &&
                geometryRectColor == other.geometryRectColor &&
                geometryRectBrush == other.geometryRectBrush &&
                childrenRectColor == other.childrenRectColor &&
                childrenRectBrush == other.childrenRectBrush &&
                transformOriginColor == other.transformOriginColor &&
                coordinatesColor == other.coordinatesColor &&
                marginsColor == other.marginsColor &&
                paddingColor == other.paddingColor &&
                gridOffset == other.gridOffset &&
                gridCellSize == other.gridCellSize &&
                gridColor == other.gridColor &&
                componentsTraces == other.componentsTraces &&
                gridEnabled == other.gridEnabled
                ;
    }

    bool operator!=(const QuickDecorationsSettings &other) const {
        return !operator==(other);
    }

    QColor boundingRectColor;
    QBrush boundingRectBrush;
    QColor geometryRectColor;
    QBrush geometryRectBrush;
    QColor childrenRectColor;
    QBrush childrenRectBrush;
    QColor transformOriginColor;
    QColor coordinatesColor;
    QColor marginsColor;
    QColor paddingColor;
    QPointF gridOffset;
    QSizeF gridCellSize;
    QColor gridColor;
    bool componentsTraces;
    bool gridEnabled;
};

QDataStream &operator<<(QDataStream &stream, const GammaRay::QuickDecorationsSettings &settings);
QDataStream &operator>>(QDataStream &stream, GammaRay::QuickDecorationsSettings &settings);

struct QuickDecorationsBaseRenderInfo {
    QuickDecorationsBaseRenderInfo(const QuickDecorationsSettings &settings = QuickDecorationsSettings(),
                                   const QRectF &viewRect = QRectF(),
                                   qreal zoom = 1.0)
        : settings(settings)
        , viewRect(viewRect)
        , zoom(zoom)
    { }

    const QuickDecorationsSettings settings;
    const QRectF viewRect;
    const qreal zoom;
};

struct QuickDecorationsRenderInfo : QuickDecorationsBaseRenderInfo {
    QuickDecorationsRenderInfo(const QuickDecorationsSettings &settings = QuickDecorationsSettings(),
                               const QuickItemGeometry &itemGeometry = QuickItemGeometry(),
                               const QRectF &viewRect = QRectF(),
                               qreal zoom = 1.0)
        : QuickDecorationsBaseRenderInfo(settings, viewRect, zoom)
        , itemGeometry(itemGeometry)
    { }

    const QuickItemGeometry itemGeometry;
};

struct QuickDecorationsTracesInfo : QuickDecorationsBaseRenderInfo {
    QuickDecorationsTracesInfo(const QuickDecorationsSettings &settings = QuickDecorationsSettings(),
                               const QVector<QuickItemGeometry> &itemsGeometry = QVector<QuickItemGeometry>(),
                               const QRectF &viewRect = QRectF(),
                               qreal zoom = 1.0)
        : QuickDecorationsBaseRenderInfo(settings, viewRect, zoom)
        , itemsGeometry(itemsGeometry)
    { }

    const QVector<QuickItemGeometry> itemsGeometry;
};

class QuickDecorationsDrawer
{
public:
    enum Type {
        Decorations,
        Traces
    };

    QuickDecorationsDrawer(QuickDecorationsDrawer::Type type, QPainter &painter,
                           const QuickDecorationsBaseRenderInfo &renderInfo);

    void render();

private:
    struct DrawTextInfo {
        DrawTextInfo(const QPen &pen = QPen(), const QRectF &rect = QRectF(),
                     const QString &label = QString(),
                     int align = Qt::AlignCenter | Qt::TextDontClip)
            : pen(pen)
            , rect(rect)
            , label(label)
            , align(align)
        { }

        QPen pen;
        QRectF rect;
        QString label;
        int align;
    };

    typedef QVector<DrawTextInfo> DrawTextInfoList;

    QuickItemGeometry itemGeometry() const;
    QVector<QuickItemGeometry> itemsGeometry() const;

    void drawDecorations();
    void drawTraces();
    void drawGrid();
    void drawArrow(const QPointF &first, const QPointF &second);
    void drawAnchor(const QuickItemGeometry &itemGeometry, Qt::Orientation orientation,
                    qreal ownAnchorLine, qreal offset);
    void drawVerticalAnchor(const QuickItemGeometry &itemGeometry, qreal ownAnchorLine,
                            qreal offset);
    void drawHorizontalAnchor(const QuickItemGeometry &itemGeometry, qreal ownAnchorLine,
                              qreal offset);
    DrawTextInfo drawAnchorLabel(const QuickItemGeometry &itemGeometry, Qt::Orientation orientation,
                                 qreal ownAnchorLine, qreal offset, const QString &label,
                                 Qt::Alignment align);
    DrawTextInfo drawHorizontalAnchorLabel(const QuickItemGeometry &itemGeometry, qreal ownAnchorLine,
                                           qreal offset, const QString &label, Qt::Alignment align);
    DrawTextInfo drawVerticalAnchorLabel(const QuickItemGeometry &itemGeometry, qreal ownAnchorLine,
                                         qreal offset, const QString &label, Qt::Alignment align);

    const QuickDecorationsDrawer::Type m_type;
    const QuickDecorationsBaseRenderInfo *const m_renderInfo;
    QPainter *const m_painter;
};
}

Q_DECLARE_METATYPE(GammaRay::QuickDecorationsSettings)

#endif
