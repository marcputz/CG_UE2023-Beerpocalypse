#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform int bloom;
uniform float exposure;
uniform float gamma;

void main() {
	vec3 hdrColor = texture(scene, TexCoords).rgb;
	vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;

	if (bloom == 1) {
		hdrColor += bloomColor; // additive blending
	}

	// tone mapping
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

	// gamma correction
	result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(result, 1.0);
}