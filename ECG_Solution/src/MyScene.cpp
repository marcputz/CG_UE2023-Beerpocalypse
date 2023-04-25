#include "MyScene.h"

MyScene::MyScene(MyFPSCamera& camera) : camera_(&camera) {
	gameObject_ = nullptr;
}

MyScene::~MyScene() {

}

void MyScene::draw() {

	for (GameObject* obj : gameObjects_) {
		obj->draw();
	}
}

void MyScene::update(float deltaTime) {
	
	for (GameObject* obj : gameObjects_) {
		obj->update(deltaTime);
	}
}

void MyScene::addGameObject(GameObject* gameObject) {
	gameObjects_.push_back(gameObject);
}
