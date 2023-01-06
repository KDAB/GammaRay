/*
  quickitemgeometry.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "quickitemgeometry.h"

#include <QQuickItem>
#include <QtNumeric>

namespace GammaRay {

bool GammaRay::QuickItemGeometry::isValid() const
{
    return !qIsNaN(x) && !qIsNaN(y);
}

void QuickItemGeometry::scaleTo(qreal factor)
{
    if (!isValid())
        return;

    itemRect = QRectF(
        itemRect.topLeft() * factor,
        itemRect.bottomRight() * factor);
    boundingRect = QRectF(
        boundingRect.topLeft() * factor,
        boundingRect.bottomRight() * factor);
    childrenRect = QRectF(
        childrenRect.topLeft() * factor,
        childrenRect.bottomRight() * factor);
    backgroundRect = QRectF(
        backgroundRect.topLeft() * factor,
        backgroundRect.bottomRight() * factor);
    contentItemRect = QRectF(
        contentItemRect.topLeft() * factor,
        contentItemRect.bottomRight() * factor);
    transformOriginPoint = transformOriginPoint * factor;
    leftMargin = leftMargin * factor;
    horizontalCenterOffset = horizontalCenterOffset * factor;
    rightMargin = rightMargin * factor;
    topMargin = topMargin * factor;
    verticalCenterOffset = verticalCenterOffset * factor;
    bottomMargin = bottomMargin * factor;
    baselineOffset = baselineOffset * factor;
    x = x * factor;
    y = y * factor;
    if (!qIsNaN(padding)) {
        padding = padding * factor;
        leftPadding = leftPadding * factor;
        rightPadding = rightPadding * factor;
        topPadding = topPadding * factor;
        bottomPadding = bottomPadding * factor;
    }
}

bool QuickItemGeometry::operator==(const QuickItemGeometry &other) const
{
    return itemRect == other.itemRect && boundingRect == other.boundingRect && childrenRect == other.childrenRect && backgroundRect == other.backgroundRect && contentItemRect == other.contentItemRect && transformOriginPoint == other.transformOriginPoint && transform == other.transform && parentTransform == other.parentTransform && x == other.x && y == other.y && left == other.left && right == other.right && top == other.top && bottom == other.bottom && horizontalCenter == other.horizontalCenter && verticalCenter == other.verticalCenter && baseline == other.baseline && margins == other.margins && leftMargin == other.leftMargin && horizontalCenterOffset == other.horizontalCenterOffset && rightMargin == other.rightMargin && topMargin == other.topMargin && verticalCenterOffset == other.verticalCenterOffset && bottomMargin == other.bottomMargin && baselineOffset == other.baselineOffset && padding == other.padding && leftPadding == other.leftPadding && rightPadding == other.rightPadding && topPadding == other.topPadding && bottomPadding == other.bottomPadding && traceColor == other.traceColor && traceTypeName == other.traceTypeName && traceName == other.traceName;
}

bool QuickItemGeometry::operator!=(const QuickItemGeometry &other) const
{
    return !operator==(other);
}

QDataStream &operator<<(QDataStream &stream, const GammaRay::QuickItemGeometry &geometry)
{
    stream << geometry.itemRect
           << geometry.boundingRect
           << geometry.childrenRect
           << geometry.backgroundRect
           << geometry.contentItemRect

           << geometry.transformOriginPoint
           << geometry.transform
           << geometry.parentTransform

           << geometry.x
           << geometry.y

           << geometry.left
           << geometry.right
           << geometry.top
           << geometry.bottom
           << geometry.horizontalCenter
           << geometry.verticalCenter
           << geometry.baseline

           << geometry.margins
           << geometry.leftMargin
           << geometry.horizontalCenterOffset
           << geometry.rightMargin
           << geometry.topMargin
           << geometry.verticalCenterOffset
           << geometry.bottomMargin
           << geometry.baselineOffset

           << geometry.padding
           << geometry.leftPadding
           << geometry.rightPadding
           << geometry.topPadding
           << geometry.bottomPadding

           << geometry.traceColor
           << geometry.traceTypeName
           << geometry.traceName;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, GammaRay::QuickItemGeometry &geometry)
{
    stream >> geometry.itemRect
        >> geometry.boundingRect
        >> geometry.childrenRect
        >> geometry.backgroundRect
        >> geometry.contentItemRect

        >> geometry.transformOriginPoint
        >> geometry.transform
        >> geometry.parentTransform

        >> geometry.x
        >> geometry.y

        >> geometry.left
        >> geometry.right
        >> geometry.top
        >> geometry.bottom
        >> geometry.horizontalCenter
        >> geometry.verticalCenter
        >> geometry.baseline

        >> geometry.margins
        >> geometry.leftMargin
        >> geometry.horizontalCenterOffset
        >> geometry.rightMargin
        >> geometry.topMargin
        >> geometry.verticalCenterOffset
        >> geometry.bottomMargin
        >> geometry.baselineOffset

        >> geometry.padding
        >> geometry.leftPadding
        >> geometry.rightPadding
        >> geometry.topPadding
        >> geometry.bottomPadding

        >> geometry.traceColor
        >> geometry.traceTypeName
        >> geometry.traceName;

    return stream;
}
}
