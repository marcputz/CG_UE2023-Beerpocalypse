#include "MyScene.h"
/*
MyScene::MyScene() {
	dynamicGameObjects_.push_back(std::make_shared<DynamicGameObject>(1));
	dynamicGameObjects_.push_back(std::make_shared<DynamicGameObject>(2));
	dynamicGameObjects_.push_back(std::make_shared<DynamicGameObject>(3));
}
*/

MyScene::MyScene(MyFPSCamera& camera) : camera_(&camera) {
	gameObject_ = nullptr;
}

MyScene::~MyScene() {

}

void MyScene::draw(MyShader& shader) {

	for (GameObject* obj : gameObjects_) {
		obj->draw(shader);
	}

	//MyShader* shader = gameObject_->getShader();
	/*
	shader->use();
	shader->setVec3("viewPos", camera_->position_);
	setUniformsOfLights(shader);
	shader->setBool("enableSpotLight", enableFlashLight);
	shader->setBool("enableNormalMapping", enableNormalMapping);

	glm::mat4 projection = glm::perspective(glm::radians(camera_->fov_), float(SCR_WIDTH) / float(SCR_HEIGHT), 0.1f, 100.0f);
	shader->setMat4("projection", projection);
	glm::mat4 view = camera_->getViewMatrix();
	shader->setMat4("view", view);
	*/

	//gameObject_->draw();

	/*
	for (std::shared_ptr<DynamicGameObject> object : dynamicGameObjects_) {
		object->draw();
	}
	*/
}

void MyScene::update(float deltaTime) {
	
	for (GameObject* obj : gameObjects_) {
		obj->update(deltaTime);
	}
	
	for (GameObject* obj : gameObjects_) {
		obj->updateSelfAndChild();
	}
}

void MyScene::addGameObject(GameObject& gameObject) {
	gameObjects_.push_back(&gameObject);
}
