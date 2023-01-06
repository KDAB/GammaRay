/*
  bindingLoopTest.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
