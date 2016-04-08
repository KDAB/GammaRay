
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.5 as QQ2
import com.kdab.GammaRay 1.0

Material {
    id: root

    property real explosionFactor : 0
    property var frontTextureImage
    property var backTextureImage
    property bool highlighted : false

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
            name: "highlighted"
            value: root.highlighted
        }
    ]


    effect: Effect {
        id: effect;

        parameters: root.parameters

        techniques: [
            Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 3
                }

                filterKeys: [
                    FilterKey {
                          name: "renderingStyle"
                          value: "forward"
                    }
                ]

                parameters: root.parameters

                renderPasses: [
                    RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: loadSource("qrc:/assets/shaders/widget.vert")
                            //geometryShaderCode: loadSource("qrc:/shaders/widget.geom")
                            fragmentShaderCode: loadSource("qrc:/assets/shaders/widget.frag")
                        }
                    }
                ]
            }
        ]
    }
}
