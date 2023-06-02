#pragma once

#include "PlayerCamera.h"

class PlayerCameraThirdPerson : public PlayerCamera {
private:
	float playerDistance = 4.0f;
	float playerYOffset = 2.0f;

public:
	PlayerCameraThirdPerson(Transform* playerTransform) : PlayerCamera(playerTransform) {}

	glm::vec3 getPosition() override;
	glm::vec3 getDirection() override;
};