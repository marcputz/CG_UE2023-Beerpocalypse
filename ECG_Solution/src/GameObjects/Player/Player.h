#pragma once
#include "../GameObject.h"
#include "../../MyFPSCamera.h"

class Player : public GameObject {
protected:
	MyFPSCamera* camera_;

public:
	void update(float deltaTime);
	void handleInput(GLFWwindow* window, float deltaTime);

	// Constructors
	Player(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo, MyFPSCamera& camera) : GameObject(shader, physics, goInfo) {
		camera_ = &camera;
	}
};

