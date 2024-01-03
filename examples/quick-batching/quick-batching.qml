/*
  quick-batching.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.0
import QtQuick.Window 2.0

Window {
    id: myWindow
    height: 600
    width: 100

    Row {
        id: myRow
        anchors.fill: parent
        anchors.margins: spacing
        spacing: 5

        //! [Slider setup]
        Slider {
            id: leftSlider
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: myWindow.width / 2 - 1.5*myRow.spacing
        }

        Slider {
            id: rightSlider
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: myWindow.width / 2 - 1.5*myRow.spacing
            mirrorSlider: true
        }
        //! [Slider setup]
    }
}
