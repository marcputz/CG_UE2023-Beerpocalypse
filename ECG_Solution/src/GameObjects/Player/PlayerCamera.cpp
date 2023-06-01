#include "PlayerCamera.h"

extern unsigned int screenWidth;
extern unsigned int screenHeight;
extern float cameraNear;
extern float cameraFar;

glm::mat4 PlayerCamera::getViewMatrix() {
	if (playerTransform != nullptr) {
		glm::vec3 playerPosition = playerTransform->getWorldPosition();
		return glm::lookAt(getPosition(), playerPosition, glm::vec3(0, 1, 0));
	}
	return glm::mat4(1.0f);
}

glm::mat4 PlayerCamera::getProjectionMatrix() {
	return glm::perspective(glm::radians(fov), float(screenWidth) / float(screenHeight), cameraNear, cameraFar);
}

glm::vec3 PlayerCamera::getPosition() {
	if (playerTransform != nullptr) {
		glm::vec3 forward = playerTransform->getForwardVector();
		glm::vec3 playerPosition = playerTransform->getWorldPosition();
		glm::vec3 cameraPosition = playerPosition - (forward * playerDistance);
		cameraPosition.y += playerYOffset;
		return cameraPosition;
	}
	return glm::vec3(0, 0, 0);
}