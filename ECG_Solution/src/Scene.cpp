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

void Scene::addObject(NewGameObject* gameObject, bool addToPhysics) {
	objects.push_back(gameObject);

	if (addToPhysics) {
		physicsScene->addActor(*(gameObject->getRigidActor()));
	}
}

void Scene::addLight(MyDirectionalLight* dirLight) {
	directionLight = dirLight;
}

void Scene::addLight(MyPointLight* pointLight) {
	pointLights.push_back(pointLight);
}

void Scene::addLight(MySpotLight* spotLight) {
	spotLights.push_back(spotLight);
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
	// Make pre-update calls
	for (NewGameObject* go : objects) {
		go->beforeUpdate();
	}

	// Make Physics update
	this->physicsScene->simulate(deltaTime);
	this->physicsScene->fetchResults(true);

	// Update Lights
	directionLight->setUniforms(0);

	int pointLightIdx = 0;
	for (MyPointLight* pointLight : pointLights) {
		pointLight->setUniforms(pointLightIdx);
		pointLightIdx++;
	}

	int spotLightIdx = 0;
	for (MySpotLight* spotLight : spotLights) {
		spotLight->setUniforms(spotLightIdx);
		spotLightIdx++;
	}

	// Call update methods
	for (NewGameObject* go : objects) {
		go->update(deltaTime);
	}
}

void Scene::draw() {
	for (NewGameObject* go : objects) {
		go->draw();
	}
}