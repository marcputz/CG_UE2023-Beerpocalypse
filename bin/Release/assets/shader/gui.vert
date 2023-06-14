#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 modelMatrix;
uniform mat4 projection;

void main() {
	//gl_Position = projection * transformationMatrix * vec4(position, 0.0, 1.0);
	TexCoords = aTexCoords;
	gl_Position = modelMatrix * vec4(aPos, 1.0);
}