#pragma once

#include "../NewGameObject.h"

class Vampire : public NewGameObject {
private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

public:
	Vampire(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);
};