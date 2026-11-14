#version 440

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 textureCoordinates;

void main()
{
    textureCoordinates = texCoord;
    gl_Position = vec4(position, 0.0, 1.0);
}
