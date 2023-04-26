#pragma once

#include "../GameObject.h"

class Cube : public GameObject {
private:

protected:

public:
	void update(float deltaTime);
	void handleInput(GLFWwindow* window, float deltaTime);

	Cube(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo);
};