#version 430
layout (location=0) in vec3 vertPos;
layout (location=1) in vec2 texCoord;
layout (location=2) in vec3 vertNormal;

out vec3 varyingNormal; // eye-space vertex normal
out vec3 varyingLightDir; // vector pointing to the light
out vec3 varyingVertPos; // vertex position in eye space
// half vector

out vec3 varyingHalfVector;
out vec2 tc;

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
uniform mat4 norm_matrix; // for transforming normals


void main(void)
{
    // output vertex position, light direction, and normal to the rasterizer for interpolation
    varyingVertPos = (mv_matrix * vec4(vertPos, 1.0)).xyz;
    varyingLightDir = light.position - varyingVertPos;
    varyingNormal = (norm_matrix * vec4(vertNormal, 1.0)).xyz;
    varyingHalfVector = (varyingLightDir -varyingVertPos).xyz;

    gl_Position = proj_matrix * mv_matrix * vec4(vertPos, 1.0);
    tc = texCoord;
}