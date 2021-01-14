/*
  shadereffect.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
