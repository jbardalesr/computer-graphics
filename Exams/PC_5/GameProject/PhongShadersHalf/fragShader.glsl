#version 430

in vec2 tc;
in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
// half vector
in vec3 varyingHalfVector;
uniform int object;
out vec4 fragColor;

layout (binding=0) uniform sampler2D samp; 

struct PositionalLight
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
};

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

void main(void)
{
    // normalize the light, normal and view vectors
    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNormal);
    vec3 V = normalize(-varyingVertPos);
    vec3 H = normalize(varyingHalfVector);

    // compute light reflection vector with respect to N
    vec3 R = normalize(reflect(-L, N));
    // get the angle between the light and surface normal
    float cosTheta = dot(L, N);
    // angle between the normal and the halfway vector
    float cosPhi = dot(H, N);

    // halfway vector H was computed in the vertex shader, and then interpolated by rasterizer
    vec3 ambient = (globalAmbient * material.ambient + light.ambient * material.ambient).xyz;
    vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);

    vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi, 0.0), material.shininess*3.0);
    // the multiplication by 3.0 at the end is a "tweak" to improve the specular highlight
    
     if (object == 0 || object == 1)
    {
        fragColor = texture(samp, tc);
    } 
    else if (object == 2)
    {
        fragColor = vec4(ambient + diffuse + specular, 1.0); 
    }
}