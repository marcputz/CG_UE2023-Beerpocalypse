#pragma once

#include "../GameObject.h"

class Cube : public GameObject {
private:

protected:

public:
	void update(float deltaTime);
	void handleKeyboardInput(GLFWwindow* window, float deltaTime);
	void handleMouseInput(float xOffset, float yOffset);

	Cube(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo);
};