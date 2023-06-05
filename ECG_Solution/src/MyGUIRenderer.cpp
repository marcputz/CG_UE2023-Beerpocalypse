#include "MyGUIRenderer.h"

MyGUIRenderer::MyGUIRenderer() {

	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO_);
	glBindVertexArray(VAO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices_), &quadVertices_, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	initTextures();
}

void MyGUIRenderer::renderGUI(MyShader& shader, int currentBeerCount, int maxBeerCount) {
	glDisable(GL_DEPTH_TEST);

	glm::vec3 position = glm::vec3(0.9f, -0.85f, 0.0f);
	// TODO: Replace with world rotation and check if it's working
	//glm::quat rotation = transform_->getLocalRotation();
	glm::vec3 scale = glm::vec3(0.05f, 0.1f, 1.0f);

	glm::mat4 transMatrix;
	//glm::mat4 rotMatrix = glm::mat4_cast(rotation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 modelMatrix(1.0f);

	shader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO_);

	beerSilhouetteTexture_.bind();

	for (int i = 0; i < (maxBeerCount - currentBeerCount); i++) {

		transMatrix = glm::translate(glm::mat4(1.0f), position);
		modelMatrix = transMatrix * scaleMatrix;

		shader.setMat4("modelMatrix", modelMatrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0., 4);

		position.x = position.x - 0.05f;
	}

	beerTexture_.bind();

	for (int i = 0; i < currentBeerCount; i++) {
		transMatrix = glm::translate(glm::mat4(1.0f), position);
		modelMatrix = transMatrix * scaleMatrix;

		shader.setMat4("modelMatrix", modelMatrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0., 4);

		position.x = position.x - 0.05f;
	}

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MyGUIRenderer::initTextures() {
	stbi_set_flip_vertically_on_load(true);
	beerTexture_ = MyAssetManager::loadTexture("assets/textures/beertransparent.png", My2DTextureTypes::DIFFUSE, true, "beerTransparent");
	beerSilhouetteTexture_ = MyAssetManager::loadTexture("assets/textures/beersilhouette.png", My2DTextureTypes::DIFFUSE, true, "beerSilhouette");
	stbi_set_flip_vertically_on_load(false);
}
