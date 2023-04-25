#include "Player.h"

float moveSpeed = 3.0f;

void Player::update(float deltaTime) {
	updateChildren(deltaTime);
}

PxVec3 glmVec3ToPxVec3(glm::vec3 vec) {
	return PxVec3(vec.x, vec.y, vec.z);
}

void Player::handleInput(GLFWwindow* window, float deltaTime) {
	PxVec3 cameraFront = glmVec3ToPxVec3(camera_->front_);
	PxVec3 cameraRight = glmVec3ToPxVec3(camera_->right_);

	PxVec3 forwardVector = PxVec3(cameraFront.x, 0, cameraFront.z).getNormalized();
	PxVec3 backwardVector = -forwardVector;
	PxVec3 leftVector = PxVec3(cameraRight.x, 0, cameraRight.z).getNormalized();
	PxVec3 rightVector = -leftVector;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		this->move(forwardVector * (moveSpeed * deltaTime));
		//camera_->processKeyboardInput(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		this->move(backwardVector * (moveSpeed * deltaTime));
		//camera_->processKeyboardInput(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		this->move(rightVector * (moveSpeed * deltaTime));
		//camera_->processKeyboardInput(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		this->move(leftVector * (moveSpeed * deltaTime));
		//camera_->processKeyboardInput(RIGHT, deltaTime);
	}
}