/*
  stackedrects.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Christoph <christoph.sterz@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

import QtQuick 2.0

Item {
  width: 100
  height: 100

  Rectangle {
    id: redrect

    color: "#ffff0000"
    x: 20
    y: 20
    width: parent.width/2
    height: parent.height/2
  }
  Rectangle {
    id: greenrect

    color: "#ff00ff00"
    x: 30
    y: 30
    width: parent.width/2
    height: parent.height/2
  }
  Rectangle {
    id:bluerect

    x: 40
    y: 40
    color: "#ff0000ff"
    width: parent.width/2
    height: parent.height/2
  }
  Rectangle {
    id: yellowrect

    x: 70
    y: 70
    color: "#ffffff00"
    width: parent.width/2
    height: parent.height/2
  }
}
