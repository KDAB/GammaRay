#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 8) out;

in VertexFormat {
    vec3 position;
    vec3 normal;
} gs_in[];

out ColorFormat {
    vec3 color;
} gs_out;

uniform mat4 mvp;

uniform float normalLength = 0.1;
uniform vec3 faceNormalColor = vec3( 1.0, 1.0, 0.0 );
uniform vec3 vertexNormalColor = vec3( 0.0, 0.0, 1.0 );

void main()
{
    // We are working in model space here since the positions were
    // just passed through form the vertex shader.

    // First, calculate the face normal and construct a line
    vec3 ba = vec3( gs_in[1].position - gs_in[0].position );
    vec3 ac = vec3( gs_in[2].position - gs_in[0].position );
    vec3 faceNormal = normalize( cross( ba, ac ) );

    // Calculate the center location of the triangle
    vec3 triangleCenter = ( gs_in[0].position +
                            gs_in[1].position +
                            gs_in[2].position ) / 3.0;

    // Emit a vertex from the triangle center and another from the
    // triangle center displaced in the direction of the face normal.
    // Remember to transform into clip-space
    gs_out.color = faceNormalColor;
    gl_Position = mvp * vec4( triangleCenter, 1.0 );
    EmitVertex();

    gs_out.color = faceNormalColor;
    gl_Position = mvp * vec4( triangleCenter + normalLength * faceNormal, 1.0 );
    EmitVertex();

    // Finish off the face normal line
    EndPrimitive();

    // Now do similar for each of the input triangle vertices but use the vertex normal
    // instead of the face normal
    for ( int i = 0; i < gl_in.length(); i++ )
    {
        // Start point of line at vertex position
        gs_out.color = vertexNormalColor;
        gl_Position = mvp * vec4( gs_in[i].position, 1.0 );
        EmitVertex();

        // End point of line offset in normal direction
        gs_out.color = vertexNormalColor;
        gl_Position = mvp * vec4( gs_in[i].position + normalLength * gs_in[i].normal, 1.0 );
        EmitVertex();

        // Finish that line off
        EndPrimitive();
    }
}
