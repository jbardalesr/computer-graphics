#version 430
layout (location=0) in vec3 vertPos;
layout (location=1) in vec3 vertNormal;

out vec3 varyingNormal; // eye-space vertex normal
out vec3 varyingLightDir; // vector pointing to the light
out vec3 varyingVertPos; // vertex location in eye space
out vec3 varyingHalfVector;// half vector
out vec4 shadow_coord;

struct Spotlights
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 location;
    vec3 direction;
    float cutoff;
    float exponent;
};

struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform vec4 globalAmbient;
uniform Spotlights light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix; // for transforming normals

uniform mat4 shadowMVP;
layout (binding=0) uniform sampler2DShadow shadowTex;

void main(void)
{
    // output vertex location, light direction, and normal to the rasterizer for interpolation
    varyingVertPos = (mv_matrix * vec4(vertPos, 1.0)).xyz;
    varyingLightDir = light.location - varyingVertPos;
    varyingNormal = (norm_matrix * vec4(vertNormal, 1.0)).xyz;
    varyingHalfVector = (varyingLightDir - varyingVertPos).xyz;

	shadow_coord = shadowMVP * vec4(vertPos,1.0);

    gl_Position = proj_matrix * mv_matrix * vec4(vertPos, 1.0);
    
}