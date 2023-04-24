#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "MyShader.h"

#define MAX_BONE_INFLUENCE 4

struct MyVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	
	glm::vec3 tangent;
	glm::vec3 bitangent;
	/*
	int boneIDs[MAX_BONE_INFLUENCE];
	float weights[MAX_BONE_INFLUENCE];
	*/
};

struct MyTexture {
	unsigned int id;
	std::string type;
	std::string path;
};

class MyMesh {
public:
	std::vector<MyVertex> vertices_;
	std::vector<unsigned int> indices_;
	std::vector<MyTexture> textures_;

	MyMesh(std::vector<MyVertex> vertices, std::vector<unsigned int> indices, std::vector<MyTexture> textures);
	void draw(MyShader& shader);

private:
	unsigned int VAO_, VBO_, EBO_;

	void setupMesh();
};
