#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexTangent;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out VertexFormat {
    vec3 position;
    vec3 normal;
    vec4 color;
} vs_out;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;

uniform int shadingMode = 0;

const int ShadingModeFlat = 0;
const int ShadingModePhong = 1;
const int ShadingModeTexture = 2;
const int ShadingModeNormal = 3;
const int ShadingModeTangent = 4;
const int ShadingModeColor = 5;
const int ShadingModeWireframe = 6;

void main()
{
    vs_out.normal = normalize( modelNormalMatrix * vertexNormal );
    vs_out.position = vertexPosition;
    switch (shadingMode) {
        case ShadingModeFlat:
            vs_out.color = vec4(0.45, 0.25, 0.15, 1.0);
            break;
        case ShadingModePhong:
            break; // handling in fragment shader
        case ShadingModeTexture:
            vs_out.color = vec4(0.0, vertexTexCoord.x, vertexTexCoord.y, 1.0);
            break;
        case ShadingModeNormal:
            vs_out.color = vec4(normalize(vertexNormal + vec3(1.0, 1.0, 1.0)), 1.0);
            break;
        case ShadingModeTangent:
            vs_out.color = vec4(normalize(vertexTangent.xyz + vec3(1.0, 1.0, 1.0)), 1.0);
            break;
        case ShadingModeColor:
            vs_out.color = vertexColor;
            break;
        case ShadingModeWireframe:
            vs_out.color = vec4(1.0, 1.0, 1.0, 0.0);
            break;
    }

    gl_Position = modelViewProjection * vec4( vertexPosition, 1.0 );
}
