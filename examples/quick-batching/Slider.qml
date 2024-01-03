/*
  Slider.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.0

pragma ComponentBehavior: Bound

Item {
    id: root

    property bool mirrorSlider: false
    property real minValue: 0
    property real maxValue: 100
    property real value: 50

    component SliderDelegate : Item {
        required property int index

        //! [Slider delegate]
        Rectangle {
            property bool active: view.currentIndex <= parent.index
            anchors.right: parent.right
            width: parent.width
            height: parent.height - 3
            color: Qt.hsva((parent.index/view.count)/3, active ? 1.0 : 0.5, active ? 1.0 : 0.75)
            opacity: active ? 1.0 : 1.0 - (view.currentIndex - parent.index) / view.model
            scale: active ? 1.0 : 0.9
        }
        //! [Slider delegate]
    }

    ListView {
        id: view
        anchors.fill: parent
        anchors.rightMargin: root.mirrorSlider ? 0 : handle.width
        anchors.leftMargin: root.mirrorSlider ? handle.width : 0

        orientation: Qt.Vertical
        interactive: false

        property int itemHeight: 7
        model: height/itemHeight

        currentIndex: (root.value - root.minValue) / (root.maxValue - root.minValue) * view.count

        delegate: SliderDelegate {
            width: view.width
            height: view.itemHeight
        }

        Rectangle {
            id: handle
            property real currentItemY: view.currentItem ? view.currentItem.y : 0
            anchors.left: root.mirrorSlider ? undefined : parent.right
            anchors.right: root.mirrorSlider ? parent.left : undefined
            width: 10
            height: width
            radius: 5
            color: "darkgray"
            y: currentItemY - height / 2 + 3
        }

        MouseArea {
            id: dragArea
            anchors.fill: view
            hoverEnabled: false
            preventStealing: true
            onClicked: (mouse)=> {
                updateCurrentIndex(mouse.x, mouse.y)
            }
            onPositionChanged: (mouse)=> {
                updateCurrentIndex(mouse.x, mouse.y)
            }

            function updateCurrentIndex(x, y) {
                var item = view.itemAt(x, y);
                if (item) {
                    var newValue = (item as SliderDelegate).index / view.count * (root.maxValue - root.minValue) + root.minValue;
                    root.value = Math.min(root.maxValue, Math.max(root.minValue, newValue));
                }
            }
        }
    }
}
