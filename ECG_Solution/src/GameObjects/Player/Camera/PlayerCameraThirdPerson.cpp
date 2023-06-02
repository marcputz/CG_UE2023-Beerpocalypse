#include "PlayerCameraThirdPerson.h"

glm::vec3 PlayerCameraThirdPerson::getPosition() {
	if (playerTransform != nullptr) {
		glm::vec3 forward = playerTransform->getForwardVector();
		glm::vec3 playerPosition = playerTransform->getWorldPosition();
		glm::vec3 cameraPosition = playerPosition - (forward * playerDistance);
		cameraPosition.y += playerYOffset;
		return cameraPosition;
	}
	return glm::vec3(0, 0, 0);
}

glm::vec3 PlayerCameraThirdPerson::getDirection() {
	if (playerTransform != nullptr) {
		glm::vec3 playerPosition = playerTransform->getWorldPosition();
		glm::vec3 cameraPosition = getPosition();

		return glm::normalize(playerPosition - cameraPosition);
	}
	return glm::vec3(0, 0, 1);
}