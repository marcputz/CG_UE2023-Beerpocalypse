#pragma once
#include "../GameObject.h"

class Player : public GameObject {
public:
	void update(float deltaTime);

	// Constructors
	Player(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo) : GameObject(shader, physics, goInfo) {}
};

