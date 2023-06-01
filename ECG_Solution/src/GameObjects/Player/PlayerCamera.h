#pragma once
#include <GameObjects/Transform.h>

class PlayerCamera {
private:
	const float playerDistance = 7.0f;
	const float playerYOffset = 2.0f;

	float fov = 90.0f;

	Transform* playerTransform;

public:
	void setPlayerTransform(Transform* playerTransform) { this->playerTransform = playerTransform; }

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	float getFov() { return fov;  }

	glm::vec3 getPosition();
	glm::vec3 getDirection();
};