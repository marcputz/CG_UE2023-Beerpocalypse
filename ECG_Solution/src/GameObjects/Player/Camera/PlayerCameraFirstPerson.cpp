#include "PlayerCameraFirstPerson.h"

glm::vec3 PlayerCameraFirstPerson::getPosition() {
	if (playerTransform != nullptr) {
		return playerTransform->getWorldPosition();
	}

	return glm::vec3(0, 0, 0);
}

glm::vec3 PlayerCameraFirstPerson::getDirection() {
	if (playerTransform != nullptr) {
		glm::vec3 direction = playerTransform->getForwardVector();
		direction.y = (yAngle / 90);
		direction = glm::normalize(direction);
		return direction;
	}

	return glm::vec3(0, 0, 1);
}

void PlayerCameraFirstPerson::processMouseInput(float offsetX, float offsetY) {
	float correctedYOffset = offsetY / 50.0f * yAngleChangeSpeed;
	yAngle += correctedYOffset;

	// set limits 
	if (yAngle < -90.0f) {
		yAngle = -90.0f;
	}
	if (yAngle > 90.0f) {
		yAngle = 90.0f;
	}
}