#pragma once

#include "../GameObject.h"

class Roof : public GameObject {
private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

	void onCollision(GameObject* otherObject) {}

public:
	Roof(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime) {};
	void processMouseInput(float offsetX, float offsetY) {};

	// Inherited via GameObject
	virtual void onHealthChange(int oldHealth, int newHealth) override;

	// Inherited via GameObject
	virtual void resetSpecifics() override;
};