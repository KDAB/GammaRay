precision highp float;

varying vec3 texCoord0;

void main()
{
    float fieldCount = 99.0;
    float colorOffset = 0.1 * mod(floor(fieldCount * texCoord0.x) + floor(fieldCount * texCoord0.y) + floor(fieldCount * texCoord0.z), 2.0);
    gl_FragColor = vec4( 0.5 + colorOffset, 0.5 + colorOffset, 0.5 + colorOffset, 1.0 );
}
