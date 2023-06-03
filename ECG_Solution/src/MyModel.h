#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb/stb_image.h>

#include "MyMesh.h"
#include "MyAssetManager.h"

struct MyAABoundingBox {
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 absDiff;
};

struct BoneInfo {
	int ID;
	glm::mat4 offset;
};

class MyModel {
public:
	MyAABoundingBox boundingBox_;

	MyModel(std::string const& path/*, bool gamma = false*/);
	void draw(MyShader& shader);

	std::map<std::string, BoneInfo>& getBoneInfoMap();
	int& getBoneCount();

	void applyTilingScale(float wScale, float hScale);

	//void create(const aiScene* scene, std::string& directory);

private:
	std::vector<MyMesh> meshes_;
	std::string directory_;
	std::map<std::string, BoneInfo> boneInfoMap_;
	int boneCounter_ = 0;

	void loadModel(std::string const &path);
	void processNode(aiNode* node, const aiScene* scene);
	MyMesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<My2DTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, My2DTextureTypes textureType);
	void setVertexBoneDataToDefault(MyVertex& vertex);
	void setVertexBoneData(MyVertex& vertex, int boneID, float weight);
	void extractBoneWeightsForVertices(std::vector<MyVertex>& vertices, aiMesh* mesh, const aiScene* scene);
};