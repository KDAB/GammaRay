/*
  WidgetDelegate.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

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


import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.5 as QQ2


Entity {
    id: root;

    property rect topLevelGeometry
    property real explosionFactor: 0

    property int level
    property var frontTextureImage
    property var backTextureImage
    property rect geometry

    readonly property real _scaleFactor : 10.0
    readonly property real _geomWidth: root.geometry.width / _scaleFactor
    readonly property real _geomHeight: root.geometry.height / _scaleFactor
    readonly property real _geomX: root.geometry.x / _scaleFactor
    readonly property real _geomY: root.geometry.y / _scaleFactor
    property real _geomZ: root.level / (_scaleFactor * 2.0) + root.level * root.explosionFactor

    QQ2.Behavior on _geomZ {
        QQ2.NumberAnimation {
            duration: 200
            easing.type: Easing.OutQuart
        }
    }


    components: [
        CuboidMesh {
            xExtent: _geomWidth
            yExtent: _geomHeight
            zExtent: 1
        },

        WidgetMaterial {
            id: material
            frontTextureImage: root.frontTextureImage
            backTextureImage: root.backTextureImage
            highlightFactor: objectPicker.containsMouse ? 0.5 : 0.0

            QQ2.Behavior on highlightFactor {
                QQ2.NumberAnimation {
                    duration: 100;
                }
            }
        },

        Transform {
            id: transform
            translation: Qt.vector3d(
                             _geomWidth / 2.0 + _geomX - topLevelGeometry.width / 2.0 / _scaleFactor,
                             -_geomHeight / 2.0 - _geomY + topLevelGeometry.height / 2.0 / _scaleFactor,
                             _geomZ
                         )
        },

        ObjectPicker {
            id: objectPicker
            hoverEnabled: true
            onClicked: console.log("Click!")
        }
    ]
}
