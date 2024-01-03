/*
  opacityzerooverlay.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Christoph <christoph.sterz@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.0


import QtQuick 2.0

Item {
  width: 100
  height: 100

  Rectangle{
      id: invisibleoverlay

      color:"#ffffffff"
      anchors.fill: parent
      opacity: 0

      Rectangle {
        id: visibleChildOfInvisibleParent

        color: "#ffff0000"
        anchors.fill: parent
        opacity: 1
      }
  }

  Rectangle {
    id: yellowrect

    color: "#ffffff00"
    anchors.centerIn: parent
    z: -40
    width: parent.width/2
    height: parent.height/2
  }
}
