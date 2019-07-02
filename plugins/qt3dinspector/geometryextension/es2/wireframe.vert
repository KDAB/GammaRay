attribute vec3 vertexPosition;

uniform mat4 modelViewProjection;

void main()
{
    gl_Position = modelViewProjection * vec4( vertexPosition, 1.0 );
}
