#pragma once
#include "../../Transform.h"

class PlayerCamera {
private:

	float fov = 75.0f;

protected:
	Transform* playerTransform;

	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::vec3(0.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f);

public:
	PlayerCamera(Transform* playerTransform) : playerTransform{ playerTransform } {}

	void setPlayerTransform(Transform* playerTransform) { this->playerTransform = playerTransform; }

	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	glm::vec3 getRight();
	glm::vec3 getUp();
	void updateRightAndUp();

	float getFov() { return fov; }
	void setFov(float newFov);

	virtual glm::vec3 getPosition() = 0;
	virtual glm::vec3 getDirection() = 0;

	virtual void processMouseInput(float offsetX, float offsetY) = 0;
};