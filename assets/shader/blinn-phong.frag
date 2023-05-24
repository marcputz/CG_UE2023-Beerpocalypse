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
vec3 calcPointLight(PointLight light, vec3 pos, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseTex, vec3 specularTex);
vec3 calcSpotLight(SpotLight light, vec3 pos, vec3 direction, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseTex, vec3 specularTex);

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentPointLightsPos[NR_POINT_LIGHTS];
    vec3 TangentSpotLightsPos[NR_SPOT_LIGHTS];
    vec3 TangentSpotLightsDir[NR_SPOT_LIGHTS];
    vec3 TangentViewPos;
    vec3 TangentFragPos;
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
		vec3 norm = texture(texture_normal1, fs_in.TexCoords).rgb;
		// transform normal vector to range [-1, 1]
		norm = normalize(norm * 2.0 - 1.0); // is in tangent space
		vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

		if (dirLight.enabled == 1) {
			result += calcDirLight(dirLight, norm, viewDir, diffuseTex, specularTex);
		}

		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			if (pointLights[i].enabled == 1) {
				result += calcPointLight(pointLights[i], fs_in.TangentPointLightsPos[i], norm, fs_in.TangentFragPos, viewDir, diffuseTex, specularTex);
			}
		}

		for (int i = 0; i < NR_SPOT_LIGHTS; i++) {
			if (spotLights[i].enabled == 1) {
				result += calcSpotLight(spotLights[i], fs_in.TangentSpotLightsPos[i], fs_in.TangentSpotLightsDir[i], norm, fs_in.TangentFragPos, viewDir, diffuseTex, specularTex);
			}
		}

		//result = norm * 0.5 + 0.5;

	} else {
		vec3 norm = normalize(fs_in.Normal);
		vec3 viewDir = normalize(viewPos - fs_in.FragPos);

		if (dirLight.enabled == 1) {
			result += calcDirLight(dirLight, norm, viewDir, diffuseTex, specularTex);
		}

		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			if (pointLights[i].enabled == 1) {
				result += calcPointLight(pointLights[i], pointLights[i].position, norm, fs_in.FragPos, viewDir, diffuseTex, specularTex);
			}
		}

		for (int i = 0; i < NR_SPOT_LIGHTS; i++) {
			if (spotLights[i].enabled == 1) {
				result += calcSpotLight(spotLights[i], spotLights[i].position, spotLights[i].direction, norm, fs_in.FragPos, viewDir, diffuseTex, specularTex);
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
	// ambient
	vec3 ambient = light.ambient * diffuseTex;

	// diffuse
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseTex;

	// specular
	// phong
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	// blinn-phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = light.specular * spec * specularTex;

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 pos, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseTex, vec3 specularTex) {
	// ambient
	vec3 ambient = light.ambient * diffuseTex;

	// diffuse
	vec3 lightDir = normalize(pos - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseTex;

	// specular
	// phong
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	// blinn-phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = light.specular * spec * specularTex;

	// attenuation (light drops off over distance)
	float distance = length(pos - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 pos, vec3 direction, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseTex, vec3 specularTex) {
	// ambient
	vec3 ambient = light.ambient * diffuseTex;

	// diffuse
	vec3 lightDir = normalize(pos - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseTex;

	// specular
	// phong
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	// blinn-phong
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = light.specular * spec * specularTex;

	// spotlight with soft edges
	float theta = dot(lightDir, normalize(-direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	ambient *= intensity;
	diffuse *= intensity;
	specular *= intensity;

	// attenuation (light drops off over distance)
	float distance = length(pos - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

/*
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	// ambient
	vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
	// diffuse
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	// ambient
	vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;
	// diffuse
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;
	// attenuation (light drops off over distance)
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	// ambient
	vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;

	// diffuse
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = light.specular * spec * texture(texture_specular1, TexCoords).rgb;

	// spotlight with soft edges
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	ambient *= intensity;
	diffuse *= intensity;
	specular *= intensity;

	// attenuation (light drops off over distance)
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}
*/