/*
  mapview.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtPositioning 5.3
import QtLocation 5.3
import QtQuick 2.15

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
                function onOverrideCoordinateChanged() {
                    sourceTrace.addCoordinate(_controller.sourceCoordinate);
                }
            }
        }

        MapPolyline {
            id: overrideTrace
            visible: _controller.overrideEnabled
            line { width: 3; color: "red" }
            opacity: 0.5
            Connections {
                target: _controller
                function onOverrideCoordinateChanged() {
                    overrideTrace.addCoordinate(_controller.overrideCoordinate);
                }
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
        function onCenterOnPosition() {
            map.center = _controller.overrideEnabled
                ? _controller.overrideCoordinate
                : _controller.sourceCoordinate;
        }
    }
}
