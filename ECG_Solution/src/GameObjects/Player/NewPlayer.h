#pragma once

#include "../NewGameObject.h"
#include "Camera/PlayerCameraThirdPerson.h"
#include "Camera/PlayerCameraFirstPerson.h"

enum PlayerCameraType {
	CAMERA_FIRST_PERSON,
	CAMERA_THIRD_PERSON
};

class NewPlayer : public NewGameObject {
private:
	const float movementSpeed = 3.0f;
	const float turningSpeed = 3.0f;

	float facingAngle = 0.0f;

	PlayerCameraType activeCameraType = PlayerCameraType::CAMERA_FIRST_PERSON;
	PlayerCameraFirstPerson* firstPersonCamera;
	PlayerCameraThirdPerson* thirdPersonCamera;

private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

	void onCollision(NewGameObject* otherObject);

public:
	// Constructors
	NewPlayer(MyShader* shader, PxPhysics* physics);

	PlayerCamera* getActiveCamera();
	void swapCamera();

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);
};

