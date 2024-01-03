/*
  shadereffect6.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import QtQuick 2.0

import QtQuick 2.0

Image {
    id: root
    source: "lsd.png"

    Text {
        anchors.centerIn: parent
        text: "Drugs are bad, mkay?"
    }

    ShaderEffectSource {
        id: effectSource
        anchors.fill: parent
        sourceItem: root
        recursive: true
        live: false
    }

    ShaderEffect {
        anchors.fill: parent
        opacity: 0.5
        rotation: 45
        scale: 2
        property variant src: effectSource
        property real customUniform: 0.5
        vertexShader: "shadereffect.vert.qsb"
        fragmentShader: "shadereffect.frag.qsb"
    }
}
