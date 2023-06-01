#pragma once

#include "../NewGameObject.h"
#include "PlayerCamera.h"

class NewPlayer : public NewGameObject {
private:
	const float movementSpeed = 3.0f;
	const float turningSpeed = 3.0f;

	float facingAngle = 0.0f;

	PlayerCamera* camera;

private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

	void onCollision(NewGameObject* otherObject);

public:
	// Constructors
	NewPlayer(MyShader* shader, PxPhysics* physics);

	PlayerCamera* getCamera();

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);
};

