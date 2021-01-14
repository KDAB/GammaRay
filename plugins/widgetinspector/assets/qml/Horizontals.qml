/*
  Horizontals.qml

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    property real geomWidth
    property real geomHeight
    property real geomX
    property real geomY
    property real geomZ
    property real explosionFactor: 0.0
    property real highlightFactor: 0.0

    readonly property real _scaleFactor: 10.0

    enabled: false

    GeometryRenderer {
        id: mesh
        instanceCount: 1
        indexOffset: 0
        firstInstance: 0
        primitiveType: GeometryRenderer.Points

        enabled: parent.enabled // FIXME: remove once "enabled" propagates correctly in Qt3D

        geometry: Geometry {
            attributes: [
                Attribute {
                    readonly property int floatSize: 4

                    attributeType: Attribute.VertexAttribute
                    vertexBaseType: Attribute.Float
                    count: 4
                    vertexSize: 3 // number of values per vertex
                    byteOffset: 0
                    byteStride: 3 * floatSize
                    name: defaultPositionAttributeName()
                    buffer: Buffer {
                        function generateVertexBufferData() {
                            var v0 = Qt.vector2d(-geomWidth / 2.0, geomHeight / 2.0); // bottom let
                            var v1 = Qt.vector2d(-geomWidth / 2.0, -geomHeight / 2.0); // top left
                            var v2 = Qt.vector2d(geomWidth / 2.0, geomHeight / 2.0); // bottom right
                            var v3 = Qt.vector2d(geomWidth / 2.0, -geomHeight / 2.0); // top right

                            // FIXME: Without the Z-axis Qt3D constantly complains
                            // that this Attribute is not suitable for volumetric object
                            // picking, despite having no ObjectPicker installed on this
                            // Entity...
                            return new Float32Array([v0.x, v0.y, 0.0,
                                                        v1.x, v1.y, 0.0,
                                                        v2.x, v2.y, 0.0,
                                                        v3.x, v3.y, 0.0]);
                        }

                        type: Buffer.VertexBuffer
                        data: generateVertexBufferData()
                    }
                }
            ]
        }
    }

    Material {
        id: material
        enabled: parent.enabled // FIXME: remove once "enabled" propagates correctly in Qt3D
        effect: Effect {
            techniques: [
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 3
                        minorVersion: 2
                    }

                    filterKeys: [
                        FilterKey {
                                name: "renderingStyle"
                                value: "forward"
                        }
                    ]

                    parameters: [
                        Parameter {
                            name: "widget.explosionFactor"
                            value: root.explosionFactor
                        },
                        Parameter {
                            name: "highlightFactor"
                            value: root.highlightFactor
                        }
                    ]

                    renderPasses: [
                        RenderPass {
                            shaderProgram: ShaderProgram {
                                vertexShaderCode: loadSource("qrc:/gammaray/assets/shaders/horizontal.vert")
                                geometryShaderCode: loadSource("qrc:/gammaray/assets/shaders/horizontal.geom")
                                fragmentShaderCode: loadSource("qrc:/gammaray/assets/shaders/horizontal.frag")
                            }
                        }
                    ]
                }
            ]
        }
    }

    Transform {
        id: transform
        enabled: parent.enabled // FIXME: remove once "enabled" propagates correctly in Qt3D
        translation: Qt.vector3d(
                        geomWidth / 2.0 + geomX - topLevelGeometry.width / 2.0 / _scaleFactor,
                        -geomHeight / 2.0 - geomY + topLevelGeometry.height / 2.0 / _scaleFactor,
                        geomZ
                    )
    }

    components: [ mesh, material, transform ]
}
