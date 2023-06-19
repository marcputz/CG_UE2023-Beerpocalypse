#include "MyAssetManager.h"

std::map<std::string, MyShader> MyAssetManager::shaders_;
std::map<std::string, My2DTexture> MyAssetManager::textures_;
irrklang::ISoundEngine* MyAssetManager::irrKlangSoundEngine_ = irrklang::createIrrKlangDevice();
std::map<std::string, irrklang::ISoundSource*> MyAssetManager::soundSources_;

Assimp::Importer MyAssetManager::importer_;

MyShader MyAssetManager::loadShader(const char* vertexShaderFileName, const char* fragmentShaderFileName, std::string shaderName) {
	// check if shader we wanna load already exists, if it does just return it
	auto search = shaders_.find(shaderName);

	if (search == shaders_.end()) {
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			std::string shaderFolder("assets/shader/");
			std::string vertexPath = shaderFolder + std::string(vertexShaderFileName);
			std::string fragmentPath = shaderFolder + std::string(fragmentShaderFileName);
			vShaderFile.open(vertexPath.c_str());
			fShaderFile.open(fragmentPath.c_str());
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		} catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		MyShader shader;
		shader.compile(vShaderCode, fShaderCode);

		shaders_[shaderName] = shader;
	}
	return shaders_[shaderName];

}

MyShader MyAssetManager::getShader(std::string shaderName) {
	return shaders_[shaderName];
}

My2DTexture MyAssetManager::loadTexture(const char* textureFileName, My2DTextureTypes textureType, bool textureAlpha, std::string textureName) {
	auto search = textures_.find(textureName);

	if (search == textures_.end()) {
		int width, height, nrChannels;
		unsigned int internalFormat = 0, imageFormat = 0;
		unsigned char* textureData = stbi_load(textureFileName, &width, &height, &nrChannels, 0);

		if (nrChannels == 1) {
			imageFormat = GL_RED;
		} else {
			if (nrChannels == 3) {
				imageFormat = GL_RGB;
			} else {
				if (nrChannels == 4) {
					imageFormat = GL_RGBA;
				}
			}
		}

		if (textureAlpha) {
			if (imageFormat == GL_RGB) {
				imageFormat = GL_RGBA;
			}
		}

		internalFormat = imageFormat;

		// sRGB for gamma correction
		if (textureType == DIFFUSE) {
			switch (imageFormat) {
				case GL_RGB:
					internalFormat = GL_SRGB;
					break;
				case GL_RGBA:
					internalFormat = GL_SRGB_ALPHA;
					break;
			}
		}

		My2DTexture texture;
		texture.generate(textureName, width, height, textureData, textureType, internalFormat, imageFormat);

		stbi_image_free(textureData);

		textures_[textureName] = texture;
	}
	return textures_[textureName];
}

My2DTexture MyAssetManager::getTexture(std::string textureName) {
	return textures_[textureName];
}

bool MyAssetManager::loadSoundSource(const char* soundSourceFileName, std::string soundName) {
	if (irrKlangSoundEngine_) {
		soundSources_[soundName] = irrKlangSoundEngine_->addSoundSourceFromFile(soundSourceFileName, irrklang::ESM_NO_STREAMING, true);

		// sounds lag the application when they are played the first time
		// so in order to avoid a lag when a sound is played during gameplay (which can break physx)
		// play the sound muted (0.0f volume) once at initial load, then set volume back to 1.0f
		soundSources_[soundName]->setDefaultVolume(0.0f);
		irrKlangSoundEngine_->play2D(soundSources_[soundName]);
		soundSources_[soundName]->setDefaultVolume(1.0f);

		return true;
	}

	return false;
}

bool MyAssetManager::playSound(std::string soundName) {
	if (irrKlangSoundEngine_ && soundSources_.count(soundName)) {
		irrKlangSoundEngine_->play2D(soundSources_[soundName]);
		return true;
	}

	return false;
}



/*
MyModel MyAssetManager::loadModel(const char* modelFileName, std::string modelName) {
	auto search = models_.find(modelName);

	if (search != models_.end()) {
		const std::string& fullPath = "assets/models/" + std::string(modelFileName);
		const aiScene* scene = importer_.ReadFile(fullPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace); //GenNormals, SplitLargeMeshes/OptimizeMeshes

		// check if errors occured during loading
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR:ASSIMP::" << importer_.GetErrorString() << std::endl;
			exit;
		}

		std::string directory = fullPath.substr(0, fullPath.find_last_of('/'));

		MyModel model;
		model.create(scene, directory);

		models_[modelName] = model;
	}

	return models_[modelName];
}

MyModel MyAssetManager::getModel(std::string modelName) {
	return models_[modelName];
}
*/

void MyAssetManager::cleanUp() {

	for (std::pair<std::string, MyShader> iter : shaders_) {
		glDeleteProgram(iter.second.ID_);
	}

	for (std::pair<std::string, My2DTexture> iter : textures_) {
		glDeleteTextures(1, &iter.second.ID_);
	}

	irrKlangSoundEngine_->drop();
}
