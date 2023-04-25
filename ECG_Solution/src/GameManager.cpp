#include "GameManager.h"
#include <GameObjects/Backpack/Backpack.h>
#include <GameObjects/GameObject.h>

GameManager::GameManager(PxPhysics* physics, MyFPSCamera& camera) : camera_(&camera) {
	// Setup Physics Scene
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	PxDefaultCpuDispatcher* gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	physicsScene_ = physics->createScene(sceneDesc);
	physics_ = physics;
}

GameManager::~GameManager() {
}

void GameManager::draw() {

	for (GameObject* obj : gameObjects_) {
		obj->draw();
	}
}

void GameManager::stepUpdate(float deltaTime) {
	for (GameObject* obj : gameObjects_) {
		obj->update(deltaTime);
	}
	stepPhysics(deltaTime);
}

void GameManager::handleInput(GLFWwindow* window, float deltaTime) {
	//camera_->handleInput(window, deltaTime);
	for (GameObject* obj : gameObjects_) {
		obj->handleInput(window, deltaTime);
	}
}

void GameManager::setPlayer(Player* player) {
	player_ = player;
}

void GameManager::addObject(GameObject* gameObject) {
	gameObjects_.push_back(gameObject);
	PxRigidActor* actor = gameObject->getActor();
	physicsScene_->addActor(*actor);
}

void GameManager::stepPhysics(float deltaTime)
{
	physicsScene_->simulate(deltaTime);
	physicsScene_->fetchResults(true);
}