#pragma once

#include "../NewGameObject.h"
#include "Camera/PlayerCameraThirdPerson.h"
#include "Camera/PlayerCameraFirstPerson.h"
#include "../Beer/Beer.h"
#include "../Zombie/Zombie.h"

enum PlayerCameraType {
	CAMERA_FIRST_PERSON,
	CAMERA_THIRD_PERSON
};

class NewPlayer : public NewGameObject {
private:
	const float movementSpeed = 4.0f;
	const float turningSpeed = 5.0f;
	const float jumpForce = 0.8f;

	float facingAngle = 0.0f;
	//bool onGround = false;

	float damageCooldown = 0.0f;
	const float targetDamageCooldown = 2.0f;

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
	PlayerCameraType getActiveCameraType();
	void swapCamera();

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);

	// Inherited via NewGameObject
	virtual void onHealthChange(int oldHealth, int newHealth) override;
};

