#version 440

layout(location = 0) out vec2 textureCoordinates;

void main()
{
    const vec2 positions[4] = vec2[](
        vec2(-1.0, -1.0),
        vec2(+1.0, -1.0),
        vec2(-1.0, +1.0),
        vec2(+1.0, +1.0)
    );
    const vec2 texCoords[4] = vec2[](
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
    );

    textureCoordinates = texCoords[gl_VertexIndex];
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
