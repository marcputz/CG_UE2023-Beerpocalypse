#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

class My2DTexture {
public:
	unsigned int ID_;
	unsigned int width_, height_;
	unsigned int internalFormat_;
	unsigned int imageFormat_;
	unsigned int wrapS_;
	unsigned int wrapT_;
	unsigned int minFilter_;
	unsigned int magFilter_;

	My2DTexture();

	void generate(unsigned int width, unsigned int height, unsigned char* textureData,
					unsigned int internalFormat = GL_RGB, unsigned int imageFormat = GL_RGB,
					unsigned int wrapS = GL_REPEAT, unsigned int wrapT = GL_REPEAT, unsigned int minFilter = GL_LINEAR,
					unsigned int magFilter = GL_LINEAR);
	void bind() const;

protected:

private:

};