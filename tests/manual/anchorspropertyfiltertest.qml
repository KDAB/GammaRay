/*
  anchorspropertyfiltertest.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
