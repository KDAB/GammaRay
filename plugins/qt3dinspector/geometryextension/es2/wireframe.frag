precision highp float;

varying vec3 position;
varying vec3 normal;
varying vec4 color;

const int MAX_LIGHTS = 8;
const int TYPE_POINT = 0;
const int TYPE_DIRECTIONAL = 1;
const int TYPE_SPOT = 2;
struct Light {
    int type;
    vec3 position;
    vec3 color;
    float intensity;
    vec3 direction;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float cutOffAngle;
};
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

const vec3 Ka = vec3(0.45, 0.25, 0.15); // Ambient reflectivity
const vec3 Kd = vec3(0.9, 0.5, 0.3); // Diffuse reflectivity
const vec3 Ks = vec3(0.09, 0.05, 0.03); // Specular reflectivity
const float shininess = 25.0; // Specular shininess factor

uniform highp int shadingMode;

const int ShadingModePhong = 1;
const int ShadingModeWireframe = 6;

vec3 adsModel( const in vec3 pos, const in vec3 normal )
{
    // Calculate the vector from the fragment to the light
    vec3 s = normalize( vec3( lights[0].position ) - pos );

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
    float dist = distance( vec3( lights[0].position ), pos );

    // Calculate light intensity when attenuation is applied
    float intensity = lights[0].intensity / ( lights[0].quadraticAttenuation * pow( dist, 2.0 ) + lights[0].linearAttenuation * dist + lights[0].constantAttenuation);

    // Combine the ambient, diffuse and specular contributions
    return intensity * ( Ka + Kd * diffuse + Ks * specular );
}

void main()
{
    if (gl_FrontFacing) {
        if (shadingMode == ShadingModePhong) {
            gl_FragColor = vec4(adsModel(position, normalize(normal)), 1.0);
        } else {
            gl_FragColor = color;
        }
    } else {
        if (shadingMode == ShadingModeWireframe) {
            gl_FragColor = color;
        } else {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    }
}
