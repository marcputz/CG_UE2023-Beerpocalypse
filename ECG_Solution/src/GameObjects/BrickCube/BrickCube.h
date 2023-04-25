#pragma once

#include <GameObjects/GameObject.h>

class BrickCube : public GameObject {
private:

protected:

public:
	void update(float deltaTime);
	void handleInput(GLFWwindow* window, float deltaTime);

	BrickCube(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo);
};