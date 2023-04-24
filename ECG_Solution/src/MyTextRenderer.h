#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <vector>
#include "Utils.h"
#include "MyShader.h"

struct MyTextCharacter {
	unsigned int textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

class MyTextRenderer {
public:
	MyTextRenderer(std::string const &fontName, unsigned int fontSize = 48);
	void renderText(MyShader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

private:
	std::map<GLchar, MyTextCharacter> characters_;
	unsigned int fontSize_;
	unsigned int VAO_, VBO_;

	void initFont(std::string const &fontName);
};