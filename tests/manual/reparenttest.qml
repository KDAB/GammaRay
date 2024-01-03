/*
  reparenttest.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.0

Rectangle {
  color: "lightsteelblue"
  width: 320
  height: width/2

  Rectangle {
    id: left
    anchors { left: parent.left; top: parent.top; margins: 20 }
    color: "yellow"
    width: 120
    height: 120

    Rectangle {
      color: "red"
      anchors.centerIn: parent
      width: 40
      height: 40
      focus: true
      Keys.onLeftPressed: { parent = left }
      Keys.onRightPressed: { parent = right }

      Rectangle {
        color: "blue"
        anchors.centerIn: parent
        anchors.margins: 5
      }
    }
  }

  Rectangle {
    id: right
    anchors { right: parent.right; top: parent.top; margins: 20 }
    color: "green"
    width: 120
    height: 120
  }
}
