/*
  quickitemcreatedestroytest.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.0

Rectangle {
  color: "lightsteelblue"
  width: 240
  height: 320

  ListView {
    id: listView
    anchors { fill: parent; margins: 20 }
    objectName: "bububu"
    model: 255
    focus: true
    delegate: Rectangle {
      color: Qt.rgba(model.index/ListView.view.count, (ListView.view.count-model.index)/ListView.view.count, 0.5, 1.0)
      implicitHeight: 20
      implicitWidth: listView.width
      Text {
        text: model.index
        anchors.fill: parent
      }
    }
  }

}
