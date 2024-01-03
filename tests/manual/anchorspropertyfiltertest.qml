/*
  anchorspropertyfiltertest.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.0

Rectangle {
    id: root
    objectName: "AnchorsPropertyFilterTest"
    color: "lightsteelblue"
    width: 200; height: 100

    Rectangle {
        id: left
        objectName: "rectWithoutAnchors"
        width: 50; height: 50
        x: 25; y: 25
        color: "red"
    }

    Rectangle {
        id: right
        objectName: "rectWithAnchors"
        width: 50; height: 50
       anchors.verticalCenter: parent.verticalCenter
       anchors.right: parent.right
       anchors.rightMargin: 25
//         x: 125; y: 25
        color: "yellow"
    }

}
