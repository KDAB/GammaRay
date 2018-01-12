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
            vs_out.color = normalize(vec4(vertexNormal.x + 1.0, vertexNormal.y + 1.0, vertexNormal.z + 1.0, 1.0));
            break;
        case ShadingModeTangent:
            vs_out.color = normalize(vec4(vertexTangent.x + 1.0, vertexTangent.y + 1.0, vertexTangent.z + 1.0, 1.0));
            break;
        case ShadingModeColor:
            vs_out.color = vertexColor;
            break;
    }

    gl_Position = modelViewProjection * vec4( vertexPosition, 1.0 );
}
