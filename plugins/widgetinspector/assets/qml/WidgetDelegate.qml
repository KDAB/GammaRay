/*
  WidgetDelegate.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/


import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0
import QtQuick 2.5 as QQ2


Entity {
    id: root;

    property rect topLevelGeometry
    property real explosionFactor: 0
    property string objectId
    property var metaData
    property bool isWindow : false

    property alias wireframe: material.wireframe
    property bool horizontals: false

    property int depth
    property var frontTextureImage
    property var backTextureImage
    property rect geometry
    property bool selected : objectPicker.containsMouse

    readonly property real _scaleFactor : 10.0
    readonly property real _geomWidth: root.geometry.width / _scaleFactor
    readonly property real _geomHeight: root.geometry.height / _scaleFactor
    readonly property real _geomX: root.geometry.x / _scaleFactor
    readonly property real _geomY: root.geometry.y / _scaleFactor
    property real _geomZ: root.depth / (_scaleFactor * 2.0) + root.depth * root.explosionFactor

    QQ2.Behavior on explosionFactor {
        QQ2.NumberAnimation {
            duration: 200
            easing.type: Easing.OutQuart
        }
    }

    property real _highlightFactor : objectPicker.containsMouse ? 0.5 : 0.0;
    QQ2.Behavior on _highlightFactor {
        QQ2.NumberAnimation {
            duration: 100;
        }
    }

    Entity {
        id: widgetCube

        CuboidMesh {
            id: cubeMesh
            xExtent: root._geomWidth
            yExtent: root._geomHeight
            zExtent: 1
        }

        WidgetMaterial {
            id: material
            frontTextureImage: root.frontTextureImage
            backTextureImage: root.backTextureImage
            explosionFactor: root.explosionFactor
            highlightFactor: root._highlightFactor
            level: root.depth
        }

        Transform {
            id: transform
            translation: Qt.vector3d(
                            _geomWidth / 2.0 + _geomX - topLevelGeometry.width / 2.0 / _scaleFactor,
                            -_geomHeight / 2.0 - _geomY + topLevelGeometry.height / 2.0 / _scaleFactor,
                            _geomZ
                        )
        }

        ObjectPicker {
            id: objectPicker
            hoverEnabled: true
        }

        components: [ cubeMesh, material, transform, objectPicker ]
    }

    Horizontals {
        id: horizontals

        enabled: !root.isWindow && root.horizontals

        geomWidth: _geomWidth
        geomHeight: _geomHeight
        geomX: _geomX
        geomY: _geomY
        geomZ: _geomZ
        explosionFactor: root.explosionFactor
        highlightFactor: root._highlightFactor
    }
}
