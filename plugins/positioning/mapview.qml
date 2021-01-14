/*
  mapview.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
            id: sourceTrace
            line { width: 3; color: "blue" }
            opacity: 0.5
            Connections {
                target: _controller
                onOverrideCoordinateChanged: sourceTrace.addCoordinate(_controller.sourceCoordinate)
            }
        }

        MapPolyline {
            id: overrideTrace
            visible: _controller.overrideEnabled
            line { width: 3; color: "red" }
            opacity: 0.5
            Connections {
                target: _controller
                onOverrideCoordinateChanged: overrideTrace.addCoordinate(_controller.overrideCoordinate)
            }
        }

        MapQuickItem {
            coordinate: _controller.sourceCoordinate
            anchorPoint { x: sourceMarker.width / 2; y: sourceMarker.height / 2 }
            sourceItem: Item {
                id: sourceMarker
                property color color: "blue"
                width: Math.abs(map.fromCoordinate(_controller.sourceCoordinate, false).x - map.fromCoordinate(_controller.sourceCoordinate.atDistanceAndAzimuth(_controller.sourceHorizontalAccuracy, 90), false).x) * 2 + 0 * map.zoomLevel
                height: width

                Rectangle {
                    anchors.fill: parent
                    color: parent.color
                    opacity: 0.25
                    radius: width/2
                }
                Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    opacity: 0.75
                    radius: width/2
                    border.width: 1
                    border.color: parent.color
                }
                Rectangle {
                    anchors.centerIn: parent
                    color: parent.color
                    width: 16
                    height: width
                    opacity: 1
                    radius: width / 2

                    Image {
                        anchors.fill: parent
                        source: "qrc:/gammaray/positioning/direction_marker.png"
                        smooth: true
                        rotation: _controller.sourceDirection
                    }
                }
            }
        }

        MapQuickItem {
            coordinate: _controller.overrideCoordinate
            anchorPoint { x: overrideMarker.width / 2; y: overrideMarker.height / 2 }
            sourceItem: Item {
                id: overrideMarker
                property color color: "red"
                width: Math.abs(map.fromCoordinate(_controller.overrideCoordinate, false).x - map.fromCoordinate(_controller.overrideCoordinate.atDistanceAndAzimuth(_controller.overrideHorizontalAccuracy, 90), false).x) * 2 + 0 * map.zoomLevel
                height: width
                visible: _controller.overrideEnabled

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
            enabled: _controller.overrideEnabled
            onClicked: _controller.overrideCoordinate = map.toCoordinate(Qt.point(mouseX, mouseY), false);
            onWheel: {
                if (wheel.modifiers & Qt.ControlModifier)
                    _controller.overrideDirection += wheel.angleDelta.y/24
                else
                    wheel.accepted = false
            }
        }
    }

    Connections {
        target: _controller
        onCenterOnPosition: map.center = _controller.overrideEnabled ? _controller.overrideCoordinate : _controller.sourceCoordinate
    }
}
