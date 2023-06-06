#include "PlayerCamera.h"

extern unsigned int screenWidth;
extern unsigned int screenHeight;
extern float cameraNear;
extern float cameraFar;

glm::mat4 PlayerCamera::getViewMatrix() {
	return glm::lookAt(getPosition(), getPosition() + getDirection(), getUp());
}

glm::mat4 PlayerCamera::getProjectionMatrix() {
	return glm::perspective(glm::radians(fov), float(screenWidth) / float(screenHeight), cameraNear, cameraFar);
}

void PlayerCamera::setFov(float newFov) {
	this->fov = newFov;
}

glm::vec3 PlayerCamera::getUp() {
	return glm::vec3(0, 1, 0);
}