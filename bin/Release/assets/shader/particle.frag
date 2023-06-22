#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;
flat in int TexSelect;
flat in float ParticleScale;


uniform sampler2D bloodTexture;
uniform sampler2D sparkleTexture;

void main() {
	vec4 result = vec4(0.0);
	//color = (texture(myTextureSampler, texCoords) * particleColor);
	if (TexSelect == 1) {
		//color = vec4(1.0, 0.0, 0.0, 1.0);
		result = texture(bloodTexture, TexCoords);
	} else {
		if (TexSelect == 2) {
			//color = vec4(0.0, 0.0, 1.0, 1.0);
			result = texture(sparkleTexture, TexCoords);
		}
	}

	result.xyz = result.xyz * (1 - ParticleScale);

	FragColor = result;
	BrightColor = result;
}