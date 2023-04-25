#pragma once
#include "../GameObject.h"
#include <PxPhysicsAPI.h>

class DynamicGameObject : public GameObject {
protected:
	// Constructors
	DynamicGameObject(const std::string& path, MyShader& shader, PxPhysics* physics);
public:
	virtual void update(float deltaTime) = 0;
	void setPosition(float x, float y, float z);
	void resetIntertia();
};

