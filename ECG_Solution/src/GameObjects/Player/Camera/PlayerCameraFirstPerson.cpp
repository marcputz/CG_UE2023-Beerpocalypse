#include "PlayerCameraFirstPerson.h"

glm::vec3 PlayerCameraFirstPerson::getPosition() {
	if (playerTransform != nullptr) {
		return playerTransform->getWorldPosition();
	}

	return glm::vec3(0, 0, 0);
}

glm::vec3 PlayerCameraFirstPerson::getDirection() {
	if (playerTransform != nullptr) {
		return playerTransform->getForwardVector();
	}

	return glm::vec3(0, 0, 1);
}