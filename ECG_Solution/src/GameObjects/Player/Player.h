#pragma once
#include "../GameObject.h"
#include "../../MyFPSCamera.h"

class Player : public GameObject {
private:
	const float turningSpeedDivisor_ = 28.0f;

protected:
	MyFPSCamera* camera_;

	// Movement
	float facingAngle_ = 0;
	PxVec3 facingVector_ = PxVec3(0, 0, -1);

public:
	void update(float deltaTime);
	void handleKeyboardInput(GLFWwindow* window, float deltaTime);
	void handleMouseInput(float xOffset, float yOffset);

	// Movement
	void setFacingAngle(float angle);
	float getFacingAngle();
	PxVec3 getFacingVector();
	glm::vec3 getGlmFacingVector();

	// Constructors
	Player(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo, MyFPSCamera& camera) : GameObject(shader, physics, goInfo) {
		camera_ = &camera;
	}
};

