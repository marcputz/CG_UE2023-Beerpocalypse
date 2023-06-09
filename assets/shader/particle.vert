#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 xyzs;
layout (location = 2) in int textureSelect;
//layout (location = 3) in vec4 color;

out vec2 texCoords;
flat out int TexSelect;
//out vec4 particleColor;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 cameraRight;
uniform vec3 cameraUp;

void main() {
	float particleSize = xyzs.w;
	vec3 pCenter = xyzs.xyz;

	vec3 vertexPos = pCenter + cameraRight * aPos.x * particleSize + cameraUp * aPos.y * particleSize;
	
	gl_Position = projection * view * vec4(vertexPos, 1.0f);
	texCoords = aPos.xy + vec2(0.5, 0.5);
	//particleColor = color;
	TexSelect = textureSelect;
}