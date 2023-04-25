#pragma once

#include <PxPhysicsAPI.h>
#include <GameObjects/GameObject.h>

class Backpack : public GameObject {
private:

protected:

public:
	void update(float deltaTime);
	void handleInput(GLFWwindow* window, float deltaTime);

	// Constructors
	Backpack(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo);
};