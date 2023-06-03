#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <string>

enum My2DTextureTypes {
	DIFFUSE,
	SPECULAR,
	NORMAL,
	HEIGHT,
	SPRITE,
	NOT_SET
};

class My2DTexture {
public:
	unsigned int ID_;
	std::string name_;
	
	My2DTextureTypes type_;
	unsigned int width_, height_;
	unsigned int internalFormat_;
	unsigned int imageFormat_;
	unsigned int wrapS_;
	unsigned int wrapT_;
	unsigned int minFilter_;
	unsigned int magFilter_;

	My2DTexture();

	void generate(std::string& name, unsigned int width, unsigned int height, unsigned char* textureData, My2DTextureTypes type,
					unsigned int internalFormat = GL_RGB, unsigned int imageFormat = GL_RGB,
					unsigned int wrapS = GL_REPEAT, unsigned int wrapT = GL_REPEAT, unsigned int minFilter = GL_LINEAR_MIPMAP_LINEAR,
					unsigned int magFilter = GL_LINEAR);
	void bind() const;

protected:

private:

};