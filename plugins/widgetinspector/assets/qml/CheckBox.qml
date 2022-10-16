/*
  CheckBox.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/


import QtQuick 2.5
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.2 as QQControls

RowLayout {
    spacing: -2

    property alias checked : chkBox.checked
    property alias text: lbl.text


    QQControls.CheckBox {
        id: chkBox
    }

    Text {
        id: lbl
        color: "white"
        MouseArea {
            anchors.fill: parent;
            onClicked: chkBox.checked = !chkBox.checked
        }
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
    }
}
