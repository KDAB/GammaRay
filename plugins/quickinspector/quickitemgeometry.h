/*
  quickitemgeometry.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKITEMGEOMETRY_H
#define GAMMARAY_QUICKINSPECTOR_QUICKITEMGEOMETRY_H

#include <QSize>
#include <QRectF>
#include <QPointF>
#include <QTransform>
#include <QColor>
#include <QMetaType>

QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE

namespace GammaRay {
struct QuickItemGeometry
{
    QuickItemGeometry()
        : x(qQNaN())
        , y(qQNaN())
        , left(false)
        , right(false)
        , top(false)
        , bottom(false)
        , horizontalCenter(false)
        , verticalCenter(false)
        , baseline(false)
        , margins(qQNaN())
        , leftMargin(qQNaN())
        , horizontalCenterOffset(qQNaN())
        , rightMargin(qQNaN())
        , topMargin(qQNaN())
        , verticalCenterOffset(qQNaN())
        , bottomMargin(qQNaN())
        , baselineOffset(qQNaN())
        , padding(qQNaN())
        , leftPadding(qQNaN())
        , rightPadding(qQNaN())
        , topPadding(qQNaN())
        , bottomPadding(qQNaN())
    {
    }

    // basic geometry
    QRectF itemRect;
    QRectF boundingRect;
    QRectF childrenRect;
    QRectF backgroundRect;
    QRectF contentItemRect;

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

    // padding
    qreal padding;
    qreal leftPadding;
    qreal rightPadding;
    qreal topPadding;
    qreal bottomPadding;

    QColor traceColor;
    QString traceTypeName;
    QString traceName;

    bool isValid() const;
    void scaleTo(qreal factor);

    bool operator==(const QuickItemGeometry &other) const;
    bool operator!=(const QuickItemGeometry &other) const;
};

QDataStream &operator<<(QDataStream &stream, const GammaRay::QuickItemGeometry &geometry);
QDataStream &operator>>(QDataStream &stream, GammaRay::QuickItemGeometry &geometry);
}

Q_DECLARE_METATYPE(GammaRay::QuickItemGeometry)
Q_DECLARE_METATYPE(QVector<GammaRay::QuickItemGeometry>)

#endif
