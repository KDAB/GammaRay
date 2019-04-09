/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Christoph Sterz<christoph.sterz@kdab.com>

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

import QtQuick 2.0

Item {
    id: root
    width: 100
    height: 100

    property alias animated: timer.running
    property alias interval: timer.interval

    Rectangle {
        color: "#ffff0000"
        anchors.top: parent.top
        anchors.left: parent.left
        width: parent.width/2
        height: parent.height/2
    }
    Rectangle {
        color: "#ff00ff00"
        anchors.top: parent.top
        anchors.right: parent.right
        width: parent.width/2
        height: parent.height/2
    }
    Rectangle {
        color: "#ff0000ff"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: parent.width/2
        height: parent.height/2
    }
    Rectangle {
        color: "#ffffff00"
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: parent.width/2
        height: parent.height/2
    }

    Timer {
        id: timer
        running: false
        repeat: true
        triggeredOnStart: true

        onTriggered: {
            var d = root.rotation;
            d += 20;
            if (d >= 360)
                d = 0;
            root.rotation = d;
        }
    }
}
