#version 330 core

in VertexData {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
} fs_in;

out vec4 fragColor;

uniform sampler2D frontTexture;
uniform sampler2D backTexture;
uniform bool highlighted;

void main(void)
{
    // front face
    vec2 coords = vec2(fs_in.texCoord.x, 1 - fs_in.texCoord.y);
    if (fs_in.normal.z > 0) {
        fragColor = texture(frontTexture, coords);
    } else if (fs_in.normal.z < 0) {
        fragColor = texture(backTexture, coords);
    } else {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    if (highlighted) {
        fragColor = mix(fragColor, vec4(0.5, 0.5, 0.5, 1.0), 0.5);
    }
}
