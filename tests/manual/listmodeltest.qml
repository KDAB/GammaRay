/*
  listmodeltest.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

import QtQuick 2.0

Rectangle {
    width: 200
    height: 100
    ListModel {
        id: cakeModel
        ListElement { name: "Mousse au chocolat"; cost: 1.29 }
        ListElement { name: "Chocolate cake"; cost: 1.99 }
        ListElement { name: "Tiramisu"; cost: 2.99 }
    }

    ListView {
        anchors.fill: parent
        model: cakeModel
        delegate: Row {
          spacing: 20
          Text { text: name }
          Text { text: cost + '€' }
        }
    }
}
