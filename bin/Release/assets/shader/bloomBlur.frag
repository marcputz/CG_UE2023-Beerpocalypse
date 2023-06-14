#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D imageToBlur;

uniform int horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main() {
	vec2 texOffset = 1.0 / textureSize(imageToBlur, 0); // get size of a texel
	vec3 result = texture(imageToBlur, TexCoords).rgb * weight[0];

	if (horizontal == 1) {
		for (int i = 1; i < 5; i++) {
			result += texture(imageToBlur, TexCoords + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
			result += texture(imageToBlur, TexCoords - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
		}
	} else {
		for (int i = 1; i < 5; i++) {
			result += texture(imageToBlur, TexCoords + vec2(texOffset.y * i, 0.0)).rgb * weight[i];
			result += texture(imageToBlur, TexCoords - vec2(texOffset.y * i, 0.0)).rgb * weight[i];
		}
	}

	FragColor = vec4(result, 1.0);
}