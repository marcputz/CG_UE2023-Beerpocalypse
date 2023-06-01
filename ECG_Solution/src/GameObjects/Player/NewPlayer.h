#pragma once

#include "../NewGameObject.h"

class NewPlayer : public NewGameObject {
private:
	const float movementSpeed = 3.0f;
	const float turningSpeed = 3.0f;

	float facingAngle = 0.0f;

private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

public:
	// Constructors
	NewPlayer(MyShader* shader, PxPhysics* physics);

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);
};

