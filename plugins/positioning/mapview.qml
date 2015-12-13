/*
  mapview.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

import QtPositioning 5.3
import QtLocation 5.3
import QtQuick 2.0

Item {
    Plugin {
        id: mapPlugin
        required.mapping: Plugin.AnyMappingFeatures
        preferred: [ "osm" ]
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        onCopyrightLinkActivated: Qt.openUrlExternally(link)

        MapPolyline {
            id: overrideTrace
            line { width: 3; color: "red" }
            opacity: 0.5
            Connections {
                target: _controller
                onOverrideCoordinateChanged: overrideTrace.addCoordinate(_controller.overrideCoordinate)
            }
        }

        MapQuickItem {
            coordinate: _controller.overrideCoordinate
            anchorPoint { x: overrideMarker.width / 2; y: overrideMarker.height / 2 }
            sourceItem: Item {
                id: overrideMarker
                property color color: "red"
                width: Math.abs(map.fromCoordinate(_controller.overrideCoordinate, false).x - map.fromCoordinate(_controller.overrideCoordinate.atDistanceAndAzimuth(_controller.overrideHorizontalAccuracy, 90), false).x) + 0 * map.zoomLevel
                height: width

                Rectangle {
                    id: accuracyBackground
                    anchors.fill: parent
                    color: parent.color
                    opacity: 0.25
                    radius: width/2
                }
                Rectangle {
                    id: accuracyBorder
                    anchors.fill: parent
                    color: "transparent"
                    opacity: 0.75
                    radius: width/2
                    border.width: 1
                    border.color: parent.color
                }
                Rectangle {
                    id: positionMarker
                    anchors.centerIn: parent
                    color: parent.color
                    width: 16
                    height: width
                    opacity: 1
                    radius: width / 2

                    Image {
                        id: directionMarker
                        anchors.fill: positionMarker
                        source: "qrc:/gammaray/positioning/direction_marker.png"
                        smooth: true
                        rotation: _controller.overrideDirection
                    }
                }
            }
        }

        MouseArea {
            acceptedButtons: Qt.LeftButton
            anchors.fill: parent
            onClicked: _controller.overrideCoordinate = map.toCoordinate(Qt.point(mouseX, mouseY), false);
        }
    }
}
