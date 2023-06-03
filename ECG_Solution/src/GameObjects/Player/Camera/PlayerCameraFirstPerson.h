#pragma once

#include "PlayerCamera.h"

class PlayerCameraFirstPerson : public PlayerCamera {
private:
	const float yAngleChangeSpeed = 1.0f;
	float yAngle = 0.0f;

public:
	PlayerCameraFirstPerson(Transform* playerTransform) : PlayerCamera(playerTransform) {}

	glm::vec3 getPosition() override;
	glm::vec3 getDirection() override;

	void processMouseInput(float offsetX, float offsetY) override;
};