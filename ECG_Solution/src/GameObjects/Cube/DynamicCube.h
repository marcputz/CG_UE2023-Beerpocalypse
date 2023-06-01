#pragma once

#include "../NewGameObject.h"

class DynamicCube : public NewGameObject {
private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

public:
	DynamicCube(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime) {};
	void processMouseInput(float offsetX, float offsetY) {};
};