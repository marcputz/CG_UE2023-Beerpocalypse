#version 330 core

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 2

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseTex, vec3 specularTex);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 diffuseTex, vec3 specularTex, vec3 fragPos);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 diffuseTex, vec3 specularTex, vec3 fragPos);

/*
vec3 calcDirLightNormalMapping();
vec3 calcPointLightNormalMapping();
vec3 calcSpotLightNormalMapping();
*/

in VS_OUT {
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
} fs_in;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
//uniform int enableSpotLight;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_height1;
uniform sampler2D texture_normal1;

uniform int enableNormalMapping;

void main() {   
	vec3 result = vec3(0.0, 0.0, 0.0);

	vec3 diffuseTex = texture(texture_diffuse1, fs_in.TexCoords).rgb;
	vec3 specularTex = texture(texture_specular1, fs_in.TexCoords).rgb;

	if (enableNormalMapping == 1) {
		// get the normal from the normalMap in the rage of [0,1]
		vec3 norm = texture(texture_normal1, fs_in.TexCoords).rgb;
		// transform normal vector to range [-1, 1]
		norm = norm * 2.0 - 1.0;
		// transform tangent-space normal to worldspace with TBN
		norm = normalize(fs_in.TBN * norm);

		vec3 viewDir = normalize(viewPos - fs_in.FragPos);

		if (dirLight.enabled == 1) {
			result += calcDirLight(dirLight, norm, viewDir, diffuseTex, specularTex);
		}

		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			if (pointLights[i].enabled == 1) {
				result += calcPointLight(pointLights[i], norm, viewDir, diffuseTex, specularTex, fs_in.FragPos);
			}
		}

		for (int i = 0; i < NR_SPOT_LIGHTS; i++) {
			if (spotLights[i].enabled == 1) {
				result += calcSpotLight(spotLights[i], norm, viewDir, diffuseTex, specularTex, fs_in.FragPos);
			}
		}

		result = norm * 0.5 + 0.5;

	} else {
		vec3 norm = normalize(fs_in.Normal);
		vec3 viewDir = normalize(viewPos - fs_in.FragPos);

		if (dirLight.enabled == 1) {
			result += calcDirLight(dirLight, norm, viewDir, diffuseTex, specularTex);
		}

		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			if (pointLights[i].enabled == 1) {
				result += calcPointLight(pointLights[i], norm, viewDir, diffuseTex, specularTex, fs_in.FragPos);
			}
		}

		for (int i = 0; i < NR_SPOT_LIGHTS; i++) {
			if (spotLights[i].enabled == 1) {
				result += calcSpotLight(spotLights[i], norm, viewDir, diffuseTex, specularTex, fs_in.FragPos);
			}
		}
	}

	FragColor = vec4(result, 1.0);

	// for bloom
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		BrightColor = vec4(FragColor.rgb, 1.0);
	} else {
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseTex, vec3 specularTex) {
	vec3 result = vec3(0.0);

	// parameters for non-ambient lighting calculations
	vec3 lightDir = normalize(-light.direction);

	// diffuse
	float diffuseCoefficient = max(dot(normal, lightDir), 0.0);

	// specular - phong
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float specularCoefficient = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

	// specular - blinn-phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float specularCoefficient = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	vec3 ambientResult = light.ambient * diffuseTex;
	vec3 diffuseResult = light.diffuse * diffuseCoefficient * diffuseTex;
	vec3 specularResult = light.specular * specularCoefficient * specularTex;

	result = (ambientResult + diffuseResult + specularResult);

	return result;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 diffuseTex, vec3 specularTex, vec3 fragPos) {
	vec3 result = vec3(0.0);

	// parameters for non-ambient lighting calculations
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse
	float diffuseCoefficient = max(dot(normal, lightDir), 0.0);

	// specular - phong
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float specularCoefficient = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

	// specular - blinn-phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float specularCoefficient = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	// attenuation (pointLight drops off over distance)
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambientResult = light.ambient * diffuseTex;
	vec3 diffuseResult = light.diffuse * diffuseCoefficient * diffuseTex;
	vec3 specularResult = light.specular * specularCoefficient * specularTex;

	// adjust lights based on the attenuation
	ambientResult *= attenuation;
	diffuseResult *= attenuation;
	specularResult *= attenuation;

	result = (ambientResult + diffuseResult + specularResult); 

	return result;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 diffuseTex, vec3 specularTex, vec3 fragPos) {
	vec3 result = vec3(0.0);

	// parameters for non-ambient lighting calculations
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse
	float diffuseCoefficient = max(dot(normal, lightDir), 0.0);

	// specular - phong
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float specularCoefficient = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

	// specular - blinn-phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float specularCoefficient = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// intensity of spotlight, with soft edges
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambientResult = light.ambient * diffuseTex;
	vec3 diffuseResult = light.diffuse * diffuseCoefficient * diffuseTex;
	vec3 specularResult = light.specular * specularCoefficient * specularTex;

	// adjust lights based on the attenuation and intensity
	float attenuationTimesIntensity = attenuation * intensity;

	ambientResult *= attenuationTimesIntensity;
	diffuseResult *= attenuationTimesIntensity;
	specularResult *= attenuationTimesIntensity;

	result = (ambientResult + diffuseResult + specularResult); 

	return result;
}
