#include "MyTextRenderer.h"

MyTextRenderer::MyTextRenderer(std::string const &fontName, unsigned int fontSize) : fontSize_(fontSize) {
	initFont(fontName);
}

void MyTextRenderer::renderText(MyShader& shader, std::string text, float x, float y, float scale, glm::vec3 color) {
	/*GLint prevPolygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &prevPolygonMode);
	if (prevPolygonMode != GL_FILL) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}*/

	shader.use();
	shader.setVec3("textColor", glm::vec3(color.x, color.y, color.z));
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO_);

	// iterate through text-string
	std::string::const_iterator iter;
	for (iter = text.begin(); iter != text.end(); iter++) {
		MyTextCharacter ch = characters_[*iter];

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;
		// update VBO for every character of string
		float vertices[6][4] = {
			{ xpos,		ypos + h,	0.0f, 0.0f },
			{ xpos,		ypos,		0.0f, 1.0f },
			{ xpos + w,	ypos,		1.0f, 1.0f },

			{ xpos,		ypos + h,	0.0f, 0.0f },
			{ xpos + w,	ypos,		1.0f, 1.0f },
			{ xpos + w,	ypos + h,	1.0f, 0.0f }
		};
		// render glyph onto the quad
		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// update VBO content
		glBindBuffer(GL_ARRAY_BUFFER, VBO_);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad with glyph
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// advance cursor for the next glyph/character (advance is the number of 1/64 pixels)
		x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//glPolygonMode(GL_FRONT_AND_BACK, prevPolygonMode);
}

void MyTextRenderer::initFont(std::string const& fontName) {

	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, ("assets/fonts/" + fontName).c_str(), 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font: " << fontName.c_str() << std::endl;
		return;
	} else {
		// set desired fontsize to load
		FT_Set_Pixel_Sizes(face, 0, fontSize_);
		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 ASCII characters
		for (unsigned char c = 0; c < 128; c++) {
			// load character
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture for character
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0, GL_RED, GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			// store created character
			MyTextCharacter character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				static_cast<unsigned int>(face->glyph->advance.x)
			};
			characters_.insert(std::pair<char, MyTextCharacter>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// we are done with freetype so we destroy it
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO_);
	glBindVertexArray(VAO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
