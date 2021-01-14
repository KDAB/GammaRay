/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import QtQuick 2.0
import QtQuick.Controls 2.0

Item {
    id: a
    width: 200
    height: 200
    property int contextPropertyFoo: 5

    Item {
        id: b
        x: 5
        y: 15
        width: x + contextPropertyFoo
        height: x < 10 ? width : y

        Component.onCompleted: {
            b.visible = Qt.binding(function(){ return x > 15 ? true : (width < y); });
        }
    }

    Item {
        id: d
        x: b.x
        y: b.x > 10 ? b.width : 6
        width: x + y
    }

    Item {
        id: c
        property bool wantBindingLoop: false
        x: b.x
        y: height
        width: x + y
        height: wantBindingLoop ? width : x
    }

    Column {
        anchors.centerIn: parent
        Button {
            text: "b.x++"
            onClicked: b.x++;
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Button {
            text: "b.x--"
            onClicked: b.x--;
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Button {
            text: "b.y++"
            onClicked: b.y++;
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Button {
            text: "Toggle binding loop"
            onClicked: c.wantBindingLoop = !c.wantBindingLoop;
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
