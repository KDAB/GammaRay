#version 330 core

in vec3 texCoord0;
out vec4 fragColor;

void main()
{
    float fieldCount = 99.0;
    float colorOffset = 0.1 * mod(floor(fieldCount * texCoord0.x) + floor(fieldCount * texCoord0.y) + floor(fieldCount * texCoord0.z), 2.0);
    fragColor = vec4( 0.5 + colorOffset, 0.5 + colorOffset, 0.5 + colorOffset, 1.0 );
}
