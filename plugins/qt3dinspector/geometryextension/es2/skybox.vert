attribute vec3 vertexPosition;
varying vec3 texCoord0;

uniform mat4 mvp;
uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseModelView;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    texCoord0 = vertexPosition.xyz;
    gl_Position = vec4(mvp * vec4(vertexPosition, 1.0)).xyww;
}
