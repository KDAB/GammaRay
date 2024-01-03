/*
  negativezordering.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Christoph <christoph.sterz@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.0

Item {
  width: 100
  height: 100

  Rectangle {
    id: redrect

    color: "#ffff0000"
    anchors.centerIn: parent
    z: -40
    width: parent.width/2
    height: parent.height/2
  }
  Rectangle {
    id: greenrect

    color: "#ff00ff00"
    anchors.centerIn: parent
    z: -4
    width: parent.width/2
    height: parent.height/2
  }
  Rectangle {
    id:bluerect

    x: 40
    y: 40
    color: "#ff0000ff"
    anchors.centerIn: parent
    z: -29
    width: parent.width/2
    height: parent.height/2
  }
  Rectangle {
    id: yellowrect

    x: 70
    y: 70
    z: -21
    color: "#ffffff00"
    anchors.centerIn: parent
    width: parent.width/2
    height: parent.height/2
  }
}
