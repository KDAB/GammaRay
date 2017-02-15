/*
  quickitemgeometry.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickitemgeometry.h"

#include <QQuickItem>

#include <private/qquickanchors_p.h>
#include <private/qquickitem_p.h>

namespace GammaRay {
void QuickItemGeometry::initFrom(QQuickItem *item)
{
    if (!item) {
        Q_ASSERT(false);
        return;
    }

    valid = true;

    QQuickItem *parent = item->parentItem();

    if (parent) {
        itemRect = item->parentItem()->mapRectToScene(
                    QRectF(item->x(), item->y(), item->width(), item->height()));
    } else {
        itemRect = QRectF(0, 0, item->width(), item->height());
    }

    boundingRect = item->mapRectToScene(item->boundingRect());
    childrenRect = item->mapRectToScene(item->childrenRect());
    transformOriginPoint = item->mapToScene(item->transformOriginPoint());

    QQuickAnchors *anchors = item->property("anchors").value<QQuickAnchors *>();

    if (anchors) {
        QQuickAnchors::Anchors usedAnchors = anchors->usedAnchors();
        left = (bool)(usedAnchors &QQuickAnchors::LeftAnchor) || anchors->fill();
        right = (bool)(usedAnchors &QQuickAnchors::RightAnchor) || anchors->fill();
        top = (bool)(usedAnchors &QQuickAnchors::TopAnchor) || anchors->fill();
        bottom = (bool)(usedAnchors &QQuickAnchors::BottomAnchor) || anchors->fill();
        baseline = (bool)(usedAnchors & QQuickAnchors::BaselineAnchor);
        horizontalCenter = (bool)(usedAnchors &QQuickAnchors::HCenterAnchor)
                                        || anchors->centerIn();
        verticalCenter = (bool)(usedAnchors &QQuickAnchors::VCenterAnchor)
                                      || anchors->centerIn();
        leftMargin = anchors->leftMargin();
        rightMargin = anchors->rightMargin();
        topMargin = anchors->topMargin();
        bottomMargin = anchors->bottomMargin();
        horizontalCenterOffset = anchors->horizontalCenterOffset();
        verticalCenterOffset = anchors->verticalCenterOffset();
        baselineOffset = anchors->baselineOffset();
        margins = anchors->margins();
    }
    x = item->x();
    y = item->y();
    QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
    transform = itemPriv->itemToWindowTransform();
    if (parent) {
        QQuickItemPrivate *parentPriv = QQuickItemPrivate::get(parent);
        parentTransform = parentPriv->itemToWindowTransform();
    }
}

void QuickItemGeometry::scaleTo(qreal factor)
{
    if (!valid)
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
    }

QDataStream &operator<<(QDataStream &stream, const GammaRay::QuickItemGeometry &geometry)
{
    stream << geometry.valid
           << geometry.itemRect
           << geometry.boundingRect
           << geometry.childrenRect

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
           << geometry.baselineOffset;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, GammaRay::QuickItemGeometry &geometry)
{
    stream >> geometry.valid
           >> geometry.itemRect
           >> geometry.boundingRect
           >> geometry.childrenRect

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
           >> geometry.baselineOffset;

    return stream;
}
}
