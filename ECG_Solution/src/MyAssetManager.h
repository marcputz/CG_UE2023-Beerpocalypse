#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>

#include "stb/stb_image.h"
#include "MyShader.h"
#include "My2DTexture.h"

class MyAssetManager {
public:
	static std::map<std::string, MyShader> shaders_;
	static std::map<std::string, My2DTexture> textures_;

	static MyShader loadShader(const char* vertexShaderFileName, const char* fragmentShaderFileName, std::string shaderName);
	static MyShader getShader(std::string shaderName);

	static My2DTexture loadTexture(const char* textureFileName, My2DTextureTypes textureType, std::string textureName);
	static My2DTexture getTexture(std::string textureName);

	/*
	static MyModel loadModel(const char* modelFileName, std::string modelName);
	static MyModel getModel(std::string modelName);
	*/

	static void cleanUp();

protected:

private:
	MyAssetManager();
	static Assimp::Importer importer_;
};