#version 330 core

#define NR_POINT_LIGHTS 4

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
    vec3 position;
	vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentPointLightsPos[NR_POINT_LIGHTS];
    vec3 TangentSpotLightPos;
    vec3 TangentSpotLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int enableNormalMapping;
//uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {

    if (enableNormalMapping == 1) {
        vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
        vs_out.TexCoords = aTexCoords;

        /* // doing it like this can cause problems
        mat3 normalMatrix = transpose(inverse(mat3(model)));
        vec3 T = normalize(normalMatrix * aTangent);
        vec3 N = normalize(normalMatrix * aNormal);
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);

        mat3 TBN = transpose(mat3(T, B, N));
        */

        // gram-schmidt process
        vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
        vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
        // re-orthogonalize T with respect to N
        T = normalize(T - dot(T, N) * N);
        // retrieve perpendicular vector B with the cross product of T and N
        vec3 B = cross(N, T);
        mat3 TBN = mat3(T, B, N);

        for (int i = 0; i < NR_POINT_LIGHTS; i++) {
            vs_out.TangentPointLightsPos[i] = TBN * pointLights[i].position;
        }
        vs_out.TangentSpotLightPos = TBN * spotLight.position;
        vs_out.TangentSpotLightDir = TBN * spotLight.direction;
        vs_out.TangentViewPos = TBN * viewPos;
        vs_out.TangentFragPos = TBN * vs_out.FragPos;

    } else {
        vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
        // COSTLY - CALCULATE ON CPU AND PASS AS UNIFORM
        vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
        vs_out.TexCoords = aTexCoords;   
    }

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}