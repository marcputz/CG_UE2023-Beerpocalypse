#include "GameManager.h"
#include "GameObjects/GameObject.h"

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
	player_->draw();
	for (GameObject* obj : gameObjects_) {
		obj->draw();
	}
}

// Game-logic and physics simulation

void GameManager::stepUpdate(float deltaTime) {
	player_->update(deltaTime);

	for (GameObject* obj : gameObjects_) {
		obj->update(deltaTime);
	}
	stepPhysics(deltaTime);

	dirLight_->setUniforms(0);

	int pointLightIdx = 0;
	for (MyPointLight* pointLight : pointLights_) {
		pointLight->setUniforms(pointLightIdx);
		pointLightIdx++;
	}

	int spotLightIdx = 0;
	for (MySpotLight* spotLight : spotLights_) {
		spotLight->setUniforms(spotLightIdx);
		spotLightIdx++;
	}

	camera_->update();
}


void GameManager::stepPhysics(float deltaTime)
{
	physicsScene_->simulate(deltaTime);
	physicsScene_->fetchResults(true);
}

// Input handling

void GameManager::handleKeyboardInput(GLFWwindow* window, float deltaTime) {
	//camera_->handleInput(window, deltaTime);
	player_->handleKeyboardInput(window, deltaTime);
	for (GameObject* obj : gameObjects_) {
		obj->handleKeyboardInput(window, deltaTime);
	}
}

void GameManager::handleMouseInput(float xOffset, float yOffset) {
	player_->handleMouseInput(xOffset, yOffset);
	for (GameObject* obj : gameObjects_) {
		obj->handleMouseInput(xOffset, yOffset);
	}
	//camera_->handleMouseInput(xOffset, yOffset);
}

// Getters/Setters and list management

void GameManager::setPlayer(Player* player) {
	player_ = player;
	//physicsScene_->addActor(*(player->getActor()));
}

void GameManager::addObject(GameObject* gameObject) {
	gameObjects_.push_back(gameObject);
	PxRigidActor* actor = gameObject->getActor();
	physicsScene_->addActor(*actor);
}

void GameManager::addLight(MyDirectionalLight* dirLight) {
	dirLight_ = dirLight;
}

void GameManager::addLight(MyPointLight* pointLight) {
	pointLights_.push_back(pointLight);
}

void GameManager::addLight(MySpotLight* spotLight) {
	spotLights_.push_back(spotLight);
}

void GameManager::setPlayerFlashLight(MySpotLight* spotLight) {
	playerFlashLight_ = spotLight;
	addLight(spotLight);
}
