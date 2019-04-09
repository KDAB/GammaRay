/*
  WidgetDelegate.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

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

import QtQuick 2.5

Item {
    id: wi

    property variant metaData: null

    function parseObjectName(o) {
        if (o.objectName != '') {
            return o.objectName + " (0x" + o.address.toString(16) + ")";
        } else {
            return "0x" + o.address.toString(16);
        }
    }

    opacity: metaData ? 1.0 : 0.0
    Behavior on opacity {
        NumberAnimation { duration: 100 }
    }

    Rectangle {
        id: background

        anchors.fill: parent

        color: "darkgray"
        border {
            width: 2
            color: "black"
        }
        opacity: 0.8
        radius: 4
    }

    height: column.height + 20
    width: column.width + 20
    Behavior on height {
        NumberAnimation { duration: 100 }
    }
    Behavior on width {
        NumberAnimation { duration: 100 }
    }

    Column {
        id: column
        height: childrenRect.height
        width: childrenRect.width
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        spacing: 2

        Text {
            text: "<b>" + (metaData ? parseObjectName(metaData) : "") + "</b>"
        }
        Text {
            text: "<b>" + qsTranslate("GammaRay::WidgetInspector::QML", "Class:")
                + "</b> " + (metaData ? metaData.className : "")
        }
        Text {
            text: "<b>" + qsTranslate("GammaRay::WidgetInspector::QML", "Parent:")
                + "</b> " + (metaData ? parseObjectName(metaData.parent) : "")
        }
        Text {
            text: "<b>" + qsTranslate("GammaRay::WidgetInspector::QML", "Position:")
                + "</b> " + (metaData ? metaData.geometry.x + ", " + metaData.geometry.y : "")
        }
        Text {
            text: "<b>" + qsTranslate("GammaRay::WidgetInspector::QML", "Size:")
                + "</b> " + (metaData ? metaData.geometry.width + " x " + metaData.geometry.height : "")
        }
    }
}
