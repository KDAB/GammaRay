
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import QtQuick 2.5 as QQ2
import com.kdab.GammaRay 1.0

Material {
    id: root

    property int level : 0
    property real explosionFactor : 0
    property var frontTextureImage
    property var backTextureImage
    property bool highlighted

    Texture2D {
        id: frontTexture;
        minificationFilter: Texture2D.LinearMipMapLinear
        magnificationFilter: Texture2D.Linear
        maximumAnisotropy: 16.0
        generateMipMaps: true

        Widget3DImageTextureImage {
            image: frontTextureImage
        }
    }

    Texture2D {
        id: backTexture;
        minificationFilter: Texture2D.LinearMipMapLinear
        magnificationFilter: Texture2D.Linear
        maximumAnisotropy: 16.0
        generateMipMaps: true

        Widget3DImageTextureImage {
            image: backTextureImage
        }
    }

    parameters: [
        Parameter { name: "explode"; value: root.explosionFactor },
        Parameter { name: "level"; value: root.level },
        Parameter { name: "frontTexture"; value: frontTexture },
        Parameter { name: "backTexture"; value: backTextureImage ? backTexture : frontTexture },
        Parameter { name: "highlighted"; value: highlighted }
    ]


    effect: Effect {
        id: effect;

        techniques: [
            Technique {
                graphicsApiFilter.api: GraphicsApiFilter.OpenGL
                graphicsApiFilter.profile: GraphicsApiFilter.CoreProfile
                graphicsApiFilter.majorVersion: 3
                graphicsApiFilter.minorVersion: 3

                renderPasses: [
                    RenderPass {
                        /*
                        renderStates: [
                            DepthMask {
                                mask: true
                            },
                            DepthTest {
                                func: DepthTest.Less
                            }
                        ]
                        */

                        bindings: [
                            ParameterMapping {
                                parameterName: "explode"
                                shaderVariableName: "explode"
                                bindingType: ParameterMapping.Uniform
                            },
                            ParameterMapping {
                                parameterName: "level"
                                shaderVariableName: "level"
                                bindingType: ParameterMapping.Uniform
                            },
                            ParameterMapping {
                                parameterName: "frontTexture"
                                shaderVariableName: "frontTexture"
                                bindingType: ParameterMapping.Uniform
                            },
                            ParameterMapping {
                                parameterName: "backTexture"
                                shaderVariableName: "backTexture"
                                bindingType: ParameterMapping.Uniform
                            },
                            ParameterMapping {
                                parameterName: "highlighted"
                                shaderVariableName: "highlighted"
                                bindingType: ParameterMapping.Uniform
                            }
                        ]

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
