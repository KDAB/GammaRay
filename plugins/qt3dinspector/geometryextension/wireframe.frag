#version 330 core

in VertexFormat {
    vec3 position;
    vec3 normal;
    noperspective vec3 edgeDistance;
} fs_in;

out vec4 fragColor;

uniform vec4 lightPosition = vec4(4.0, 4.0, 4.0, 1.0);
uniform vec3 lightIntensity = vec3(1.0, 1.0, 1.0);
uniform float lightAc = 1.0; // Constant attenuation
uniform float lightAl = 0.01; // Linear attenuation
uniform float lightAq = 0.005; // Quadratic attenuation

uniform vec3 Ka = vec3(0.1, 0.1, 0.1); // Ambient reflectivity
uniform vec3 Kd = vec3(0.9, 0.5, 0.3); // Diffuse reflectivity
uniform vec3 Ks = vec3(0.1, 0.1, 0.1); // Specular reflectivity
uniform float shininess = 100; // Specular shininess factor

uniform float lineWidth = 0.4;
uniform vec4 lineColor = vec4(0.4, 1.0, 0.8, 1.0);

vec3 adsModel( const in vec3 pos, const in vec3 normal )
{
    // Calculate the vector from the fragment to the light
    vec3 s = normalize( vec3( lightPosition ) - pos );

    // Calculate the vector from the fragment to the eye position (the
    // origin since this is in "eye" or "camera" space
    vec3 v = normalize( -pos );

    // Refleft the light beam using the normal at this fragment
    vec3 r = reflect( -s, normal );

    // Calculate the diffus component
    vec3 diffuse = vec3( max( dot( s, normal ), 0.0 ) );

    // Calculate the specular component
    vec3 specular = vec3( pow( max( dot( r, v ), 0.0 ), shininess ) ) * ( shininess + 2.0 ) / 2.0;

    // Calculate distance from the fragment to the light
    float dist = distance( vec3( lightPosition ), pos );

    // Calculate light intensity when attenuation is applied
    vec3 intensity = lightIntensity / ( lightAq * pow( dist, 2.0 ) + lightAl * dist + lightAc );

    // Combine the ambient, diffuse and specular contributions
    return intensity * ( Ka + Kd * diffuse + Ks * specular );
}

void main()
{
    // Calculate the color from the phong model
    vec4 color;
    if (gl_FrontFacing)
        color = vec4( adsModel( fs_in.position, normalize( fs_in.normal ) ), 1.0 );
    else
        color = vec4(1.0, 0.0, 0.0, 1.0);

    // Find the smallest distance between the fragment and a triangle edge
    float d = min( fs_in.edgeDistance.x, fs_in.edgeDistance.y );
    d = min( d, fs_in.edgeDistance.z );

    // Blend between line color and phong color
    float mixVal;
    if ( d < lineWidth - 1 )
    {
        mixVal = 1.0;
    }
    else if ( d > lineWidth + 1 )
    {
        mixVal = 0.0;
    }
    else
    {
        float x = d - ( lineWidth - 1 );
        mixVal = exp2( -2.0 * ( x * x ) );
    }

    fragColor = mix( color, lineColor, mixVal );
}
