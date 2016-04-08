#version 330 core

in VertexData {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
} fs_in;

out vec4 fragColor;

uniform sampler2D frontTexture;
uniform sampler2D backTexture;
uniform float highlightFactor;

void main(void)
{
    vec2 coords = vec2(fs_in.texCoord.x, 1 - fs_in.texCoord.y);
    // front face
    if (fs_in.normal.z > 0) {
        fragColor = texture(frontTexture, coords);
    // back face
    } else if (fs_in.normal.z < 0) {
        fragColor = texture(backTexture, coords);
    // side face
    } else {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }

    fragColor = mix(fragColor, vec4(0.5, 0.5, 0.5, 1.0), highlightFactor);
}
