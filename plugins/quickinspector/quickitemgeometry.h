/*
  quickitemgeometry.h

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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKITEMGEOMETRY_H
#define GAMMARAY_QUICKINSPECTOR_QUICKITEMGEOMETRY_H

#include <QSize>
#include <QRectF>
#include <QPointF>
#include <QTransform>

namespace GammaRay {

struct QuickItemGeometry {
    // basic geometry
    QRectF itemRect;
    QRectF boundingRect;
    QRectF childrenRect;

    // transform
    QPointF transformOriginPoint;
    QTransform transform;
    QTransform parentTransform;


    // simple position
    qreal x;
    qreal y;

    // anchors
    bool left;
    bool right;
    bool top;
    bool bottom;
    bool horizontalCenter;
    bool verticalCenter;
    bool baseline;

    // margins
    qreal margins;
    qreal leftMargin;
    qreal horizontalCenterOffset;
    qreal rightMargin;
    qreal topMargin;
    qreal verticalCenterOffset;
    qreal bottomMargin;
    qreal baselineOffset;
};

}

QDataStream& operator<<(QDataStream &stream, const GammaRay::QuickItemGeometry &geometry);
QDataStream& operator>>(QDataStream &stream, GammaRay::QuickItemGeometry &geometry);

Q_DECLARE_METATYPE(GammaRay::QuickItemGeometry)

#endif

