#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform int isHighlighted;
uniform vec3 highlightColor;

void main(){    
	vec4 result = texture(texture_diffuse1, TexCoords);

	if (isHighlighted == 1) {
		result.rgb = result.rgb * highlightColor;
	}

    FragColor = result;

	// for bloom
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		BrightColor = vec4(FragColor.rgb, 1.0);
	} else {
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
}