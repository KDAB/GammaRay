/*
  WidgetMaterial.qml

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/



import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.5 as QQ2
import com.kdab.GammaRay 1.0

Material {
    id: root

    property real explosionFactor: 0
    property real highlightFactor: 0
    property int level: 0
    property bool wireframe: false

    property var frontTextureImage
    property var backTextureImage

    Texture2D {
        id: frontTexture;
        minificationFilter: Texture2D.LinearMipMapLinear
        magnificationFilter: Texture2D.Linear
        maximumAnisotropy: 16.0
        generateMipMaps: true

        Widget3DImageTextureImage {
            image: root.frontTextureImage
        }
    }

    Texture2D {
        id: backTexture;
        minificationFilter: Texture2D.LinearMipMapLinear
        magnificationFilter: Texture2D.Linear
        maximumAnisotropy: 16.0
        generateMipMaps: true

        Widget3DImageTextureImage {
            image: root.backTextureImage
        }
    }

    effect: Effect {
        id: effect;

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
                        name: "frontTexture"
                        value: frontTexture
                    },
                    Parameter {
                        name: "backTexture"
                        value: root.backTextureImage ? backTexture : frontTexture
                    },
                    Parameter {
                        name: "highlightFactor"
                        value: root.highlightFactor
                    }
                ]

                renderPasses: [
                    RenderPass {
                        id: texturePass
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/gammaray/assets/shaders/widget.vert")
                            fragmentShaderCode: loadSource("qrc:/gammaray/assets/shaders/widget.frag")
                        }
                    },

                    RenderPass {
                        id: wireframePass
                        enabled: wireframe
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/gammaray/assets/shaders/widget_wireframe.vert")
                            geometryShaderCode: loadSource("qrc:/gammaray/assets/shaders/widget_wireframe.geom")
                            fragmentShaderCode: loadSource("qrc:/gammaray/assets/shaders/widget_wireframe.frag")
                        }
                    }
                ]
            }
        ]
    }
}
