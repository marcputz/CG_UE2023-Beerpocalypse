#include "MyModel.h"

MyModel::MyModel(std::string const& path){
	const std::string& fullPath = "assets/models/" + path;
	loadModel(fullPath);
}

void MyModel::draw(MyShader& shader) {
	for (unsigned int i = 0; i < meshes_.size(); i++) {
		meshes_[i].draw(shader);
	}
}

std::map<std::string, BoneInfo>& MyModel::getBoneInfoMap() {
	return boneInfoMap_;
}

int& MyModel::getBoneCount() {
	return boneCounter_;
}

void MyModel::loadModel(std::string const &path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace/* | aiProcess_GenBoundingBoxes*/); //GenNormals, SplitLargeMeshes/OptimizeMeshes

	// check if errors occured during loading
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR:ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory_ = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

	boundingBox_.absDiff = glm::vec3(std::abs(boundingBox_.max.x - boundingBox_.min.x), std::abs(boundingBox_.max.y - boundingBox_.min.y), std::abs(boundingBox_.max.z - boundingBox_.min.z));
}

/*
void MyModel::create(const aiScene* scene, std::string& directory) {
	directory_ = directory;

	processNode(scene->mRootNode, scene);
}
*/

void MyModel::processNode(aiNode* node, const aiScene* scene) {

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}

}

MyMesh MyModel::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<MyVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<My2DTexture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		// vertex positions
		MyVertex vertex;

		setVertexBoneDataToDefault(vertex);

		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		boundingBox_.min.x = std::min(boundingBox_.min.x, vector.x);
		boundingBox_.min.y = std::min(boundingBox_.min.y, vector.y);
		boundingBox_.min.z = std::min(boundingBox_.min.z, vector.z);

		boundingBox_.max.x = std::max(boundingBox_.max.x, vector.x);
		boundingBox_.max.y = std::max(boundingBox_.max.y, vector.y);
		boundingBox_.max.z = std::max(boundingBox_.max.z, vector.z);

		// add normals to the vertices
		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}

		// add texture coords to vertices
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;

			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent = vector;

			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = vector;

		} else {
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// vertex indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	extractBoneWeightsForVertices(vertices, mesh, scene);

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<My2DTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE);
	for (My2DTexture map : diffuseMaps) {
		textures.push_back(map);
	}

	std::vector<My2DTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR);
	for (My2DTexture map : specularMaps) {
		textures.push_back(map);
	}

	std::vector<My2DTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, NORMAL);
	for (My2DTexture map : normalMaps) {
		textures.push_back(map);
	}

	std::vector<My2DTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, HEIGHT);
	for (My2DTexture map : heightMaps) {
		textures.push_back(map);
	}

	return MyMesh(vertices, indices, textures);
}

std::vector<My2DTexture> MyModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, My2DTextureTypes textureType) {
	std::vector<My2DTexture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		std::string pathToTextureFile = directory_ + '/' + std::string(str.C_Str());
		// alpha = false since we do not check in any way if the texture of a model should be translucent
		My2DTexture tex = MyAssetManager::loadTexture(pathToTextureFile.c_str(), textureType, false, pathToTextureFile);

		textures.push_back(tex);
	}

	return textures;
}

void MyModel::setVertexBoneDataToDefault(MyVertex& vertex) {
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		vertex.boneIDs[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}

void MyModel::setVertexBoneData(MyVertex& vertex, int boneID, float weight) {
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		if (vertex.boneIDs[i] < 0) {
			vertex.boneIDs[i] = boneID;
			vertex.weights[i] = weight;
			break;
		}
	}
}

void MyModel::extractBoneWeightsForVertices(std::vector<MyVertex>& vertices, aiMesh* mesh, const aiScene* scene) {
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (boneInfoMap_.find(boneName) == boneInfoMap_.end()) {
			BoneInfo newBoneInfo;
			newBoneInfo.ID = boneCounter_;
			newBoneInfo.offset = glm::mat4(
				mesh->mBones[boneIndex]->mOffsetMatrix.a1, mesh->mBones[boneIndex]->mOffsetMatrix.b1, mesh->mBones[boneIndex]->mOffsetMatrix.c1, mesh->mBones[boneIndex]->mOffsetMatrix.d1,
				mesh->mBones[boneIndex]->mOffsetMatrix.a2, mesh->mBones[boneIndex]->mOffsetMatrix.b2, mesh->mBones[boneIndex]->mOffsetMatrix.c2, mesh->mBones[boneIndex]->mOffsetMatrix.d2,
				mesh->mBones[boneIndex]->mOffsetMatrix.a3, mesh->mBones[boneIndex]->mOffsetMatrix.b3, mesh->mBones[boneIndex]->mOffsetMatrix.c3, mesh->mBones[boneIndex]->mOffsetMatrix.d3,
				mesh->mBones[boneIndex]->mOffsetMatrix.a4, mesh->mBones[boneIndex]->mOffsetMatrix.b4, mesh->mBones[boneIndex]->mOffsetMatrix.c4, mesh->mBones[boneIndex]->mOffsetMatrix.d4
			);
			boneInfoMap_[boneName] = newBoneInfo;
			boneID = boneCounter_;
			boneCounter_++;
		} else {
			boneID = boneInfoMap_[boneName].ID;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; weightIndex++) {
			int vertexID = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexID <= vertices.size());
			setVertexBoneData(vertices[vertexID], boneID, weight);
		}
	}
}

