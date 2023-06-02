#pragma once
#include <GameObjects/Transform.h>

class PlayerCamera {
private:

	float fov = 90.0f;

protected:
	Transform* playerTransform;

public:
	PlayerCamera(Transform* playerTransform) : playerTransform{ playerTransform } {}

	void setPlayerTransform(Transform* playerTransform) { this->playerTransform = playerTransform; }

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	float getFov() { return fov; }

	virtual glm::vec3 getPosition() = 0;
	virtual glm::vec3 getDirection() = 0;

	glm::vec3 getUp();
};