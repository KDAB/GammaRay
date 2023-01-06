/*
  quick-batching.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
