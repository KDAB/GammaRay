#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
} gs_in[];

out VertexData {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
} gs_out;

uniform mat4 viewportMatrix;
uniform mat4 mvp;
uniform float explode;
uniform int level;


void main()
{
    for (int i = 0; i < gl_in.length(); i++) {
        vec4 pos = vec4(gs_in[i].position, 1);
        //pos.y = pos.y - level * explode;
        gl_Position = mvp * pos;
        gs_out.position = gs_in[i].position;
        gs_out.normal = gs_in[i].normal;
        gs_out.texCoord = gs_in[i].texCoord;

        EmitVertex();
    }
    EndPrimitive();
}
