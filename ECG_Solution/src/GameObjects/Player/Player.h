#pragma once

#include "../GameObject.h"
#include "Camera/PlayerCameraThirdPerson.h"
#include "Camera/PlayerCameraFirstPerson.h"
#include "../Beer/Beer.h"
#include "../Zombie/Zombie.h"

enum PlayerCameraType {
	CAMERA_FIRST_PERSON,
	CAMERA_THIRD_PERSON
};

class Player : public GameObject {
private:
	const float movementSpeed = 5.0f;
	const float turningSpeed = 9.0f;
	const float jumpForce = 5.0f;

	float facingAngle = 0.0f;
	bool onGround = true;

	glm::vec3 respawnPoint_;

	float damageCooldown = 0.0f;
	const float targetDamageCooldown = 0.5f;

	const float FOOTSTEP_SOUND_DELAY = 0.6f;
	bool playFootstepSound = false;
	float footstepTimeCounter = 0.0f;

	PlayerCameraType activeCameraType = PlayerCameraType::CAMERA_FIRST_PERSON;
	PlayerCameraFirstPerson* firstPersonCamera;
	PlayerCameraThirdPerson* thirdPersonCamera;

private:
	void onBeforeUpdate();
	void onUpdate(float deltaTime);

	void onCollision(GameObject* otherObject);

public:
	// Constructors
	Player(MyShader* shader, PxPhysics* physics);

	PlayerCamera* getActiveCamera();
	PlayerCameraType getActiveCameraType();
	void swapCamera();
	bool isOnGround();
	void setIsOnGround(bool newValue);

	void processWindowInput(GLFWwindow* window, float deltaTime);
	void processMouseInput(float offsetX, float offsetY);

	void processMouseScrolling(double yOffset);

	// Inherited via NewGameObject
	virtual void onHealthChange(int oldHealth, int newHealth) override;

	void setRespawnPoint(glm::vec3& newRespawnPoint);
	void setCameraFOVs(float newFov);
	void jump();

	// Inherited via NewGameObject
	virtual void resetSpecifics() override;
};

