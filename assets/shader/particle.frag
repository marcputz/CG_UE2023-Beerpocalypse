#version 330 core

in vec2 texCoords;

in vec4 particleColor;

out vec4 color;

uniform sampler2D myTextureSampler;

void main() {
	color = (texture(myTextureSampler, texCoords) * particleColor);
}