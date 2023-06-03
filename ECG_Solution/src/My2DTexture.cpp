#include "My2DTexture.h"

My2DTexture::My2DTexture() : type_(NOT_SET), width_(0), height_(0), internalFormat_(GL_RGB), imageFormat_(GL_RGB), wrapS_(GL_REPEAT), wrapT_(GL_REPEAT), minFilter_(GL_LINEAR_MIPMAP_LINEAR), magFilter_(GL_LINEAR) {
	ID_ = 0;
}

void My2DTexture::generate(unsigned int width, unsigned int height, unsigned char* textureData, My2DTextureTypes type, unsigned int internalFormat, unsigned int imageFormat, unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int magFilter) {
	width_ = width;
	height_ = height;
	type_ = type;
	internalFormat_ = internalFormat;
	imageFormat_ = imageFormat;
	wrapS_ = wrapS;
	wrapT_ = wrapT;
	minFilter_ = minFilter;
	magFilter_ = magFilter;

	glGenTextures(1, &ID_);

	glBindTexture(GL_TEXTURE_2D, ID_);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat_, width_, height_, 0, imageFormat_, GL_UNSIGNED_BYTE, textureData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter_);

	// anisotropy to make mipmap textures less blurry
	float anisotropy = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void My2DTexture::bind() const {
	glBindTexture(GL_TEXTURE_2D, ID_);
}
