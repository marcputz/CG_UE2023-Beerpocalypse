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

glm::vec3 PlayerCamera::getRight() {
	return this->cameraRight;
}

glm::vec3 PlayerCamera::getUp() {
	return this->cameraUp;
}

void PlayerCamera::updateRightAndUp() {
	this->cameraRight = glm::normalize(glm::cross(this->worldUp, getDirection()));
	this->cameraUp = glm::cross(getDirection(), this->cameraRight);
}
