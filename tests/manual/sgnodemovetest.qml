/*
  sgnodemovetest.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick

Rectangle {
    color: "lightsteelblue"
    width: 320
    height: 160

    Item {
        objectName: "container"
        anchors.fill: parent

        Repeater {
            model: 6
            Rectangle {
                x: index * 40
                width: 30
                height: 30
                color: "red"
            }
        }
    }
}
