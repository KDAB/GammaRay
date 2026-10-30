/*
  outofviewtest.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick

Item {
    width: 200
    height: 200

    // out of view, and its child with it
    Rectangle {
        objectName: "outside"
        x: 300
        width: 50
        height: 50
        Rectangle {
            objectName: "outsideChild"
            width: 10
            height: 10
        }
    }

    // out of view, but not visible anyway
    Rectangle {
        objectName: "hiddenOutside"
        x: 300
        width: 50
        height: 50
        visible: false
    }

    // outside of its parent, but in the window, like the header of an ApplicationWindow
    Item {
        y: 100
        width: 200
        height: 100
        Rectangle {
            objectName: "aboveParent"
            y: -50
            width: 50
            height: 40
        }
    }

    // delegates scrolled out of a view are fine
    ListView {
        width: 200
        height: 40
        model: 20
        delegate: Rectangle {
            objectName: "delegate" + index
            width: 200
            height: 20
        }
    }
}
