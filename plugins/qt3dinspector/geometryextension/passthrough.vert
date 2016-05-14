#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;

out VertexFormat {
    vec3 position;
    vec3 normal;
} vs_out;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;

void main()
{
    vs_out.normal = normalize( modelNormalMatrix * vertexNormal );
    vs_out.position = vertexPosition;

    gl_Position = modelViewProjection * vec4( vertexPosition, 1.0 );
}
