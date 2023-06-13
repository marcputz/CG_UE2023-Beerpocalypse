#version 330 core

//#define NR_POINT_LIGHTS 4
//#define NR_SPOT_LIGHTS 2

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 aBoneIds; 
layout (location = 6) in vec4 aWeights;

struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
    int enabled;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
    int enabled;
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
    int enabled;
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
    /*
    vec3 TangentPointLightsPos[NR_POINT_LIGHTS];
    vec3 TangentSpotLightsPos[NR_SPOT_LIGHTS];
    vec3 TangentSpotLightsDir[NR_SPOT_LIGHTS];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    */
    mat3 TBN;
} vs_out;

//uniform DirLight dirLight;
//uniform PointLight pointLights[NR_POINT_LIGHTS];
//uniform SpotLight spotLights[NR_SPOT_LIGHTS];

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int enableNormalMapping;
//uniform vec3 lightPos;
uniform vec3 viewPos;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
    mat4 boneTransform = finalBonesMatrices[aBoneIds[0]] * aWeights[0];
    boneTransform += finalBonesMatrices[aBoneIds[1]] * aWeights[1];
    boneTransform += finalBonesMatrices[aBoneIds[2]] * aWeights[2];
    boneTransform += finalBonesMatrices[aBoneIds[3]] * aWeights[3];

    vec4 totalPosition = boneTransform * vec4(aPos, 1.0f);
    //mat3 normalMatrix = mat3(transpose(inverse(boneTransform)));
    mat3 transposeInverseModelTimesBoneTransform = mat3(transpose(inverse(model))) * mat3(boneTransform);

    if (enableNormalMapping == 1) {

        vs_out.FragPos = vec3(model * totalPosition);
        vs_out.TexCoords = aTexCoords;
        vs_out.Normal = transposeInverseModelTimesBoneTransform * aNormal;

        // Gram-Schmidt process to re-othogonalize the vectors
        vec3 T = normalize(transposeInverseModelTimesBoneTransform * aTangent); // normalize(vec3(transpose(inverse(model)) * boneTransform * vec4(aTangent, 0.0)));
        vec3 N = normalize(transposeInverseModelTimesBoneTransform * aNormal); // normalize(vec3(transpose(inverse(model)) * boneTransform * vec4(aNormal, 0.0)));
        // re-orthogonalize tangent with normal
        T = normalize(T - dot(T, N) * N);
        vec3 B = cross(N, T);

        vs_out.TBN = mat3(T, B, N);
    } else {
        vs_out.FragPos = vec3(model * totalPosition);
        vs_out.Normal = transposeInverseModelTimesBoneTransform * aNormal; // mat3(transpose(inverse(model))) * mat3(boneTransform) * aNormal;
        vs_out.TexCoords = aTexCoords;
    }
    
    gl_Position =  projection * view * model * totalPosition;
}