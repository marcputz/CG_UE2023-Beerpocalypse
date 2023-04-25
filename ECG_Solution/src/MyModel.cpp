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

void MyModel::loadModel(std::string const &path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes); //GenNormals, SplitLargeMeshes/OptimizeMeshes

	// check if errors occured during loading
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR:ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory_ = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
	aiAABB bb = scene->mMeshes[0]->mAABB;
	boundingBox_.min = glm::vec3(bb.mMin.x, bb.mMin.y, bb.mMin.z);
	boundingBox_.max = glm::vec3(bb.mMax.x, bb.mMax.y, bb.mMax.z);
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
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

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
		My2DTexture tex = MyAssetManager::loadTexture(pathToTextureFile.c_str(), textureType, pathToTextureFile);

		textures.push_back(tex);
	}

	return textures;
}

/*
unsigned int MyModel::textureFromFile(const char* path, const std::string& directory) {
	return 0;
}
*/
