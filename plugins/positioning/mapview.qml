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

        MapQuickItem {
            coordinate: _controller.overrideCoordinate
            anchorPoint { x: overrideMarker.width / 2; y: overrideMarker.height / 2 }
            sourceItem: Rectangle {
                id: overrideMarker
                width: Math.abs(map.fromCoordinate(_controller.overrideCoordinate, false).x - map.fromCoordinate(_controller.overrideCoordinate.atDistanceAndAzimuth(_controller.overrideHorizontalAccuracy, 90), false).x) + 0 * map.zoomLevel
                height: width
                color: "red"
                opacity: 0.5
                radius: width/2
                border { width: 2; color: "red" }

                Rectangle {
                  anchors.centerIn: parent
                  color: parent.color
                  width: 5
                  height: width
                  opacity: 1
                  radius: width / 2
                }
            }
        }

        MapPolyline {
            id: overrideTrace
            line { width: 3; color: "red" }
            opacity: 0.5
            Connections {
                target: _controller
                onOverrideCoordinateChanged: overrideTrace.addCoordinate(_controller.overrideCoordinate)
            }
        }

    }
}
