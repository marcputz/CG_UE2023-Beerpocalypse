#include "Scene.h"

Scene::Scene(PxPhysics* physics) {
	// Setup Physics Scene
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	PxDefaultCpuDispatcher* gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	this->physicsScene = physics->createScene(sceneDesc);
	this->physics = physics;
}

void Scene::addObject(NewGameObject* gameObject) {
	objects.push_back(gameObject);
}

void Scene::handleKeyboardInput(GLFWwindow* window, float deltaTime) {
	for (NewGameObject* go : objects) {
		go->processWindowInput(window, deltaTime);
	}
}

void Scene::handleMouseInput(float xOffset, float yOffset) {
	for (NewGameObject* go : objects) {
		go->processMouseInput(xOffset, yOffset);
	}
}

void Scene::step(float deltaTime) {
	for (NewGameObject* go : objects) {
		go->beforeUpdate();
	}

	this->physicsScene->simulate(deltaTime);
	this->physicsScene->fetchResults(true);

	for (NewGameObject* go : objects) {
		go->update(deltaTime);
	}
}

void Scene::draw() {
	for (NewGameObject* go : objects) {
		go->draw();
	}
}