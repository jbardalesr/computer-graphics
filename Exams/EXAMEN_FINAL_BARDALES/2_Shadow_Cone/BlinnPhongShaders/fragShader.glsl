#version 430
in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 varyingHalfVector;

out vec4 fragColor;
uniform vec4 color_obj;
uniform float is_golden;
in vec4 shadow_coord;


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
uniform mat4 norm_matrix;

float toRadians(float degrees) { return (degrees * 3.14159f) / 180.0f; }

uniform mat4 shadowMVP;
layout (binding=0) uniform sampler2DShadow shadowTex;

float lookup(float x, float y)
{  	
	// determina si en es punto hay sombra para realizar el promedio, ver imagen
	// -0.01 el componente z sirve para darle esa profundidad de sombra
	// a shadow_coord le adicionamos quiero que me diga si en ese punto que estoy tratando de crear si es sombra o no.
	// si usamos 64 tenemos una sombra mas adecuada
	float t = textureProj(shadowTex, shadow_coord + vec4(x * 0.001 * shadow_coord.w,
                                                         y * 0.001 * shadow_coord.w,
                                                         -0.01, 0.0));
	return t;
}


void main(void)
{
    // normalize the light, normal and view vectors
    float shadowFactor = 0.0;

    vec3 L = normalize(varyingLightDir);
    vec3 N = normalize(varyingNormal);
    vec3 V = normalize(-varyingVertPos);
    vec3 H = normalize(varyingHalfVector); 

 	float swidth = 4.0;
	// consigo la coordenada del fragmento, gl_FragCord coordenada del fragmento, en la ventana de 600x600
	vec2 o = mod(floor(gl_FragCoord.xy), 2.0) * swidth;
	shadowFactor += lookup(-1.5*swidth + o.x,  1.5*swidth - o.y);
	shadowFactor += lookup(-1.5*swidth + o.x, -0.5*swidth - o.y);
	shadowFactor += lookup( 0.5*swidth + o.x,  1.5*swidth - o.y);
	shadowFactor += lookup( 0.5*swidth + o.x, -0.5*swidth - o.y);
	// 
	shadowFactor = shadowFactor / 4.0;

    // get the angle between the light and surface normal
    float cosTheta = dot(L, N);
    // angle between the normal and the halfway vector
    float cosPhi = dot(H, N);

	float theta = dot(normalize(-light.direction), -L);

    vec4 diffuse = light.diffuse;
    vec4 specular = light.specular;

    float cutoff_outer = cos(toRadians(12.0));

    float epsilon   = light.cutoff - cutoff_outer;
    float intensity = clamp((theta - cutoff_outer) / epsilon, 0.0, 1.0);    

    diffuse = 0.7 - diffuse*intensity;
    specular = 0.7 - specular*intensity;

    fragColor = vec4(0.0, 0.0, 0.0, 0.0);

    vec4 shadowColor = globalAmbient * material.ambient + light.ambient * material.ambient;
    vec4 lightedColor;
    
    if (is_golden == 0.0f)
    {
        lightedColor = color_obj*(diffuse * material.diffuse * max(dot(L,N),0.0) + specular * material.specular* pow(max(dot(H,N),0.0),material.shininess*3.0));
    }
    else
    {
        lightedColor = diffuse * material.diffuse * max(dot(L,N),0.0) + specular * material.specular* pow(max(dot(H,N),0.0),material.shininess*3.0);
    }
    
    
    if (theta  > light.cutoff)
    {
        fragColor = vec4((shadowColor.xyz + shadowFactor*(lightedColor.xyz)),1.0);
        //fragColor = globalAmbient + light.ambient * color_obj + diffuse * color_obj * max(cosTheta,0.0) + specular * color_obj * pow(max(cosPhi,0.0), material.shininess);
    }
}


