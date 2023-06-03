#include "MyMesh.h"

MyMesh::MyMesh(std::vector<MyVertex> vertices, std::vector<unsigned int> indices, std::vector<My2DTexture> textures) {
	this->vertices_ = vertices;
	this->indices_ = indices;
	this->textures_ = textures;

	setupMesh();
}

void MyMesh::draw(MyShader& shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for (unsigned int i = 0; i < textures_.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name;

		if (textures_[i].type_ == DIFFUSE) {
			name = "texture_diffuse";
			number = std::to_string(diffuseNr++);
		} else {
			if (textures_[i].type_ == SPECULAR) {
				name = "texture_specular";
				number = std::to_string(specularNr++);
			} else {
				if (textures_[i].type_ == NORMAL) {
					name = "texture_normal";
					number = std::to_string(normalNr++);
				} else {
					if (textures_[i].type_ == HEIGHT) {
						name = "texture_height";
						number = std::to_string(heightNr++);
					}
				}
			}
		}
		shader.setInt((name + number).c_str(), i);
		textures_[i].bind();
	}

	glBindVertexArray(VAO_);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void MyMesh::applyTilingScaleToUVCoordinates(float wScale, float hScale) {
	for (int i = 0; i < vertices_.size(); i++) {
		vertices_[i].texCoords.x = vertices_[i].texCoords.x * wScale;
		vertices_[i].texCoords.y = vertices_[i].texCoords.y * hScale;
	}

	glBindVertexArray(VAO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);

	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(MyVertex), &vertices_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);
	// vertex texture coords 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)offsetof(MyVertex, texCoords));

	glBindVertexArray(0);
	
}

void MyMesh::setupMesh() {
	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO_);
	glGenBuffers(1, &EBO_);

	glBindVertexArray(VAO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);

	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(MyVertex), &vertices_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)offsetof(MyVertex, normal));
	// vertex texture coords 
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)offsetof(MyVertex, texCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)offsetof(MyVertex, tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)offsetof(MyVertex, bitangent));
	// boneIds
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(MyVertex), (void*)offsetof(MyVertex, boneIDs));
	// weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)offsetof(MyVertex, weights));
	
	glBindVertexArray(0);
}
