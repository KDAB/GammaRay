/*
  quick-event-handling.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Window 2.0

Window {
    id: myWindow
    height: 100
    width: 2 * height

    //! [Button setup]
    Button {
        id: myButton
        anchors.fill: parent
        anchors.margins: 20
        text: "Click me!"
        onClicked: console.log("Button clicked!")
    }
    //! [Button setup]

    //! [Hidden button]
    Button {
        id: hiddenButton
        anchors.fill: parent
        anchors.leftMargin: parent.width / 2
        text: "Hidden button"
        opacity: 0
    }
    //! [Hidden button]
}
