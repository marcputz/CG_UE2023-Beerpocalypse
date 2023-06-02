#pragma once

#include "PlayerCamera.h"

class PlayerCameraFirstPerson : public PlayerCamera {
public:
	PlayerCameraFirstPerson(Transform* playerTransform) : PlayerCamera(playerTransform) {}

	glm::vec3 getPosition() override;
	glm::vec3 getDirection() override;
};