#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexFormat {
    vec3 position;
    vec3 normal;
    vec4 color;
} gs_in[];

out VertexFormat {
    vec3 position;
    vec3 normal;
    noperspective vec3 edgeDistance;
    vec4 color;
} gs_out;

uniform mat4 viewportMatrix;

void main()
{
    // Transform each vertex into viewport space
    vec2 p0 = vec2( viewportMatrix * ( gl_in[0].gl_Position / gl_in[0].gl_Position.w ) );
    vec2 p1 = vec2( viewportMatrix * ( gl_in[1].gl_Position / gl_in[1].gl_Position.w ) );
    vec2 p2 = vec2( viewportMatrix * ( gl_in[2].gl_Position / gl_in[2].gl_Position.w ) );

    // Calculate lengths of 3 edges of triangle
    float a = length( p1 - p2 );
    float b = length( p2 - p0 );
    float c = length( p1 - p0 );

    // Calculate internal angles using the cosine rule
    float alpha = acos( ( b * b + c * c - a * a ) / ( 2.0 * b * c ) );
    float beta = acos( ( a * a + c * c - b * b ) / ( 2.0 * a * c ) );

    // Calculate the perpendicular distance of each vertex from the opposing edge
    float ha = abs( c * sin( beta ) );
    float hb = abs( c * sin( alpha ) );
    float hc = abs( b * sin( alpha ) );

    // Now add this perpendicular distance as a per-vertex property in addition to
    // the position and normal calculated in the vertex shader.

    // Vertex 0 (a)
    gs_out.edgeDistance = vec3( ha, 0, 0 );
    gs_out.normal = gs_in[0].normal;
    gs_out.position = gs_in[0].position;
    gs_out.color = gs_in[0].color;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    // Vertex 1 (b)
    gs_out.edgeDistance = vec3( 0, hb, 0 );
    gs_out.normal = gs_in[1].normal;
    gs_out.position = gs_in[1].position;
    gs_out.color = gs_in[1].color;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    // Vertex 2 (c)
    gs_out.edgeDistance = vec3( 0, 0, hc );
    gs_out.normal = gs_in[2].normal;
    gs_out.position = gs_in[2].position;
    gs_out.color = gs_in[2].color;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    // Finish the primitive off
    EndPrimitive();
}
