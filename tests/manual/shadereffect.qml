/*
  shadereffect.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
        vertexShader: "
            /* TESTVERTEXSHADER */
            uniform highp mat4 qt_Matrix;
            attribute highp vec4 qt_Vertex;
            attribute highp vec2 qt_MultiTexCoord0;
            varying highp vec2 coord;
            void main() {
                coord = qt_MultiTexCoord0;
                gl_Position = qt_Matrix * qt_Vertex;
            }"
        fragmentShader: "
            /* TESTFRAGMENTSHADER */
            varying highp vec2 coord;
            uniform sampler2D src;
            uniform lowp float qt_Opacity;
            uniform lowp float customUniform;
            void main() {
                lowp vec4 tex = texture2D(src, coord);
                gl_FragColor = vec4(vec3(dot(tex.rgb, vec3(0.344, customUniform, 0.156))), tex.a) * qt_Opacity;
            }"
    }
}
