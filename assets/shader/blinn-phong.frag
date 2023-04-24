#version 330 core

#define NR_POINT_LIGHTS 4

out vec4 FragColor;

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

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 diffuseTex, vec3 specularTex);
vec3 calcPointLight(PointLight light, vec3 pos, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseTex, vec3 specularTex);
vec3 calcSpotLight(SpotLight light, vec3 pos, vec3 direction, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseTex, vec3 specularTex);

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentPointLightsPos[NR_POINT_LIGHTS];
    vec3 TangentSpotLightPos;
    vec3 TangentSpotLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform int enableSpotLight;
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
		vec3 norm = texture(texture_height1, fs_in.TexCoords).rgb;
		// transform normal vector to range [-1, 1]
		norm = normalize(norm * 2.0 - 1.0); // is in tangent space
		vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

		result += calcDirLight(dirLight, norm, viewDir, diffuseTex, specularTex);

		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			result += calcPointLight(pointLights[i], fs_in.TangentPointLightsPos[i], norm, fs_in.TangentFragPos, viewDir, diffuseTex, specularTex);
		}

		if (enableSpotLight == 1) {
			result += calcSpotLight(spotLight, fs_in.TangentSpotLightPos, fs_in.TangentSpotLightDir, norm, fs_in.TangentFragPos, viewDir, diffuseTex, specularTex);
		}

	} else {
		vec3 norm = normalize(fs_in.Normal);
		vec3 viewDir = normalize(viewPos - fs_in.FragPos);

		result += calcDirLight(dirLight, norm, viewDir, diffuseTex, specularTex);

		for (int i = 0; i < NR_POINT_LIGHTS; i++) {
			result += calcPointLight(pointLights[i], pointLights[i].position, norm, fs_in.FragPos, viewDir, diffuseTex, specularTex);
		}

		if (enableSpotLight == 1) {
			result += calcSpotLight(spotLight, spotLight.position, spotLight.direction, norm, fs_in.FragPos, viewDir, diffuseTex, specularTex);
		}
	}
	FragColor = vec4(result, 1.0);
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