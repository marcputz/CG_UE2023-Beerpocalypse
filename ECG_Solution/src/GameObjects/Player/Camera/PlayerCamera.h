#pragma once
#include "../../Transform.h"

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
	void setFov(float newFov);

	virtual glm::vec3 getPosition() = 0;
	virtual glm::vec3 getDirection() = 0;

	virtual void processMouseInput(float offsetX, float offsetY) = 0;

	glm::vec3 getUp();
};