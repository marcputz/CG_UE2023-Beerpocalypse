#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb/stb_image.h>
#include "MyMesh.h"


class MyModel {
public:
	MyModel(std::string const& path/*, bool gamma = false*/);
	void draw(MyShader& shader);

private:
	std::vector<MyMesh> meshes_;
	std::string directory_;
	std::vector<MyTexture> textures_loaded_;
	//bool gammaCorrection_;

	void loadModel(std::string const &path);
	void processNode(aiNode* node, const aiScene* scene);
	MyMesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<MyTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int textureFromFile(const char* path, const std::string& directory/*, bool gamma = false*/);
};