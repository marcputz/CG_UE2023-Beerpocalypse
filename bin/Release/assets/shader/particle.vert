#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aXYZS;
layout (location = 2) in int aTextureSelect;
//layout (location = 3) in vec4 color;

out vec2 TexCoords;
flat out int TexSelect;
flat out float ParticleScale;
//out vec4 particleColor;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 cameraRight;
uniform vec3 cameraUp;

void main() {
	float particleSize = aXYZS.w;
	vec3 particleCenter = aXYZS.xyz;

	vec3 vertexPos = particleCenter + cameraRight * aPos.x * particleSize + cameraUp * aPos.y * particleSize;
	
	gl_Position = projection * view * vec4(vertexPos, 1.0f);
	TexCoords = aPos.xy + vec2(0.5, 0.5);
	TexSelect = aTextureSelect;
	ParticleScale = particleSize;
	//particleColor = color;
}