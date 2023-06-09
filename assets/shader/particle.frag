#version 330 core

in vec2 texCoords;
flat in int TexSelect;
//in vec4 particleColor;

out vec4 color;

uniform sampler2D myTextureSampler;

void main() {
	//color = (texture(myTextureSampler, texCoords) * particleColor);
	if (TexSelect == 1) {
		color = vec4(1.0, 0.0, 0.0, 1.0);
	} else {
		if (TexSelect == 2) {
			color = vec4(0.0, 0.0, 1.0, 1.0);
		}
	}
}