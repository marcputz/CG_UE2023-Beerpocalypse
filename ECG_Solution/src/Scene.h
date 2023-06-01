#pragma once

#include "GameObjects/NewGameObject.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <Lights/PointLight/MyPointLight.h>
#include <Lights/SpotLight/MySpotLight.h>
#include <Lights/DirectionalLight/MyDirectionalLight.h>

class Scene {
private:
	vector<NewGameObject*> objects = {};

	MyDirectionalLight* directionLight;
	vector<MyPointLight*> pointLights = {};
	vector<MySpotLight*> spotLights = {};

	PxPhysics* physics;
	PxScene* physicsScene;

public:
	Scene(PxPhysics* physics);

	void addObject(NewGameObject* gameObject);
	void addLight(MyDirectionalLight* dirLight);
	void addLight(MyPointLight* pointLight);
	void addLight(MySpotLight* spotLight);

	void handleKeyboardInput(GLFWwindow* window, float deltaTime);
	void handleMouseInput(float xOffset, float yOffset);

	void step(float deltaTime);
	void draw();
};