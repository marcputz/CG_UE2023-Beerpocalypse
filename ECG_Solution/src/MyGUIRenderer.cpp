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

void MyGUIRenderer::renderGUI(MyShader& shader, int currentBeerCount, int maxBeerCount, int currentBulletCount, int maxBulletCount, int currentHealth, int maxHealth) {
	glDisable(GL_DEPTH_TEST);

	renderBeers(shader, currentBeerCount, maxBeerCount);
	renderBullets(shader, currentBulletCount, maxBulletCount);
	renderHealth(shader, currentHealth, maxHealth);

	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MyGUIRenderer::renderBeers(MyShader& shader, int currentBeerCount, int maxBeerCount) {
	glm::vec3 position = glm::vec3(0.925f, -0.875f, 0.0f);
	// TODO: Replace with world rotation and check if it's working
	//glm::quat rotation = transform_->getLocalRotation();
	glm::vec3 scale = glm::vec3(0.0425f, 0.1f, 1.0f);

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

		position.x = position.x - 0.0425f;
	}

	beerTexture_.bind();

	for (int i = 0; i < currentBeerCount; i++) {
		transMatrix = glm::translate(glm::mat4(1.0f), position);
		modelMatrix = transMatrix * scaleMatrix;

		shader.setMat4("modelMatrix", modelMatrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0., 4);

		position.x = position.x - 0.0425f;
	}
}

void MyGUIRenderer::renderBullets(MyShader& shader, int currentBulletCount, int maxBulletCount) {

	glm::vec3 position = glm::vec3(-0.925f + (maxBulletCount* 0.015241f), -0.8f, 0.0f);
	// TODO: Replace with world rotation and check if it's working
	//glm::quat rotation = transform_->getLocalRotation();
	glm::vec3 scale = glm::vec3(0.015241f, 0.05f, 1.0f);

	glm::mat4 transMatrix;
	//glm::mat4 rotMatrix = glm::mat4_cast(rotation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 modelMatrix(1.0f);

	shader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO_);

	bulletTexture_.bind();

	for (int i = 0; i < currentBulletCount; i++) {
		transMatrix = glm::translate(glm::mat4(1.0f), position);
		modelMatrix = transMatrix * scaleMatrix;

		shader.setMat4("modelMatrix", modelMatrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0., 4);

		position.x = position.x - 0.0305f;
	}

	bulletSilhouetteTexture_.bind();

	for (int i = 0; i < (maxBulletCount - currentBulletCount); i++) {

		transMatrix = glm::translate(glm::mat4(1.0f), position);
		modelMatrix = transMatrix * scaleMatrix;

		shader.setMat4("modelMatrix", modelMatrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0., 4);

		position.x = position.x - 0.0305f;
	}
}

void MyGUIRenderer::renderHealth(MyShader& shader, int currentHealth, int maxHealth) {
	if (maxHealth == 0) {
		return;
	}

	glm::vec3 position = glm::vec3(-0.9f, -0.925f, 0.0f);
	// TODO: Replace with world rotation and check if it's working
	//glm::quat rotation = transform_->getLocalRotation();
	glm::vec3 scale = glm::vec3(0.075f * ((float)currentHealth/(float)maxHealth), 0.05f, 1.0f);

	glm::mat4 transMatrix;
	//glm::mat4 rotMatrix = glm::mat4_cast(rotation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 modelMatrix(1.0f);

	shader.use();

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO_);

	healthTexture_.bind();

	transMatrix = glm::translate(glm::mat4(1.0f), position);
	modelMatrix = transMatrix * scaleMatrix;

	shader.setMat4("modelMatrix", modelMatrix);

	glDrawArrays(GL_TRIANGLE_STRIP, 0., 4);
}

void MyGUIRenderer::initTextures() {
	stbi_set_flip_vertically_on_load(true);
	beerTexture_ = MyAssetManager::loadTexture("assets/textures/beertransparent.png", My2DTextureTypes::DIFFUSE, true, "beerTransparent");
	beerSilhouetteTexture_ = MyAssetManager::loadTexture("assets/textures/beersilhouette.png", My2DTextureTypes::DIFFUSE, true, "beerSilhouette");
	bulletTexture_ = MyAssetManager::loadTexture("assets/textures/bullettransparent.png", My2DTextureTypes::DIFFUSE, true, "bulletTransparent");
	bulletSilhouetteTexture_ = MyAssetManager::loadTexture("assets/textures/bulletsilhouette.png", My2DTextureTypes::DIFFUSE, true, "bulletSilhouette");
	healthTexture_ = MyAssetManager::loadTexture("assets/textures/lifebar.png", My2DTextureTypes::DIFFUSE, true, "lifebar");
	stbi_set_flip_vertically_on_load(false);
}
