#pragma once

#include "../GameObject.h"

class Vampire : public GameObject {
private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

	void onCollision(GameObject* otherObject) {}

public:
	Vampire(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);

	// Inherited via GameObject
	virtual void onHealthChange(int oldHealth, int newHealth) override;

	// Inherited via GameObject
	virtual void resetSpecifics() override;
};