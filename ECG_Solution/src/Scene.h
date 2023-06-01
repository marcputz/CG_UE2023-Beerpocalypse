#pragma once

#include "GameObjects/NewGameObject.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>

class Scene {
private:
	vector<NewGameObject*> objects = {};

	PxPhysics* physics;
	PxScene* physicsScene;

public:
	Scene(PxPhysics* physics);

	void addObject(NewGameObject* gameObject);

	void handleKeyboardInput(GLFWwindow* window, float deltaTime);
	void handleMouseInput(float xOffset, float yOffset);

	void step(float deltaTime);
	void draw();
};