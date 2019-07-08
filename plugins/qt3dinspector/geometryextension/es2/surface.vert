attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec4 vertexTangent;
attribute vec2 vertexTexCoord;
attribute vec4 vertexColor;

varying vec3 position;
varying vec3 normal;
varying vec4 color;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;

uniform int shadingMode;

const int ShadingModeFlat = 0;
const int ShadingModePhong = 1;
const int ShadingModeTexture = 2;
const int ShadingModeNormal = 3;
const int ShadingModeTangent = 4;
const int ShadingModeColor = 5;
const int ShadingModeWireframe = 6;

void main()
{
    position = vertexPosition;
    normal = normalize(modelNormalMatrix * vertexNormal);

    if (shadingMode == ShadingModeFlat) {
        color = vec4(0.45, 0.25, 0.15, 1.0);
    } else if (shadingMode == ShadingModePhong) {
        // handling in fragment shader
    } else if (shadingMode == ShadingModeTexture) {
        color = vec4(0.0, vertexTexCoord.x, vertexTexCoord.y, 1.0);
    } else if (shadingMode == ShadingModeNormal) {
        color = vec4(normalize(vertexNormal + vec3(1.0, 1.0, 1.0)), 1.0);
    } else if (shadingMode == ShadingModeTangent) {
        color = vec4(normalize(vertexTangent.xyz + vec3(1.0, 1.0, 1.0)), 1.0);
    } else if (shadingMode == ShadingModeColor) {
        color = vertexColor;
    } else if (shadingMode == ShadingModeWireframe) {
        color = vec4(1.0, 1.0, 1.0, 0.0);
    }

    gl_Position = modelViewProjection * vec4( vertexPosition, 1.0 );
}
