#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

out VertexData {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
} vs_out;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 mvp;

void main(void)
{
    vs_out.position = vertexPosition;
    vs_out.normal = vertexNormal;
    vs_out.texCoord = vertexTexCoord;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
