/*
  CheckBox.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
