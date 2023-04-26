#include "Player.h"

float moveSpeed = 3.0f;

void Player::update(float deltaTime) {
	updateChildren(deltaTime);
}

PxVec3 glmVec3ToPxVec3(glm::vec3 vec) {
	return PxVec3(vec.x, vec.y, vec.z);
}

void Player::handleKeyboardInput(GLFWwindow* window, float deltaTime) {
	PxVec3 forwardVector = PxVec3(facingVector_.x, 0, facingVector_.z).getNormalized();
	PxVec3 backwardVector = -forwardVector;

	PxVec3 leftFacingVector = facingVector_.cross(PxVec3(0, 1, 0)).getNormalized();
	PxVec3 leftVector = PxVec3(leftFacingVector.x, 0, leftFacingVector.z).getNormalized();
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

	//camera_->setPosition(getGlmPosition());
}

void Player::handleMouseInput(float xOffset, float yOffset) {
	setFacingAngle(facingAngle_ + (-xOffset / turningSpeedDivisor_));
}

// Movement

void Player::setFacingAngle(float angle) {

	while (angle < 0) {
		angle += 360;
	}
	while (angle > 360) {
		angle -= 360;
	}

	facingAngle_ = angle;
	facingVector_ = PxVec3(-sin(glm::radians(angle)), 0, -cos(glm::radians(angle))).getNormalized();

	// Update player rotation
	PxVec3 rotationAxis(0, 1, 0);
	PxQuat newRotation(glm::radians(facingAngle_), rotationAxis.getNormalized());

	PxTransform transform = actor_->getGlobalPose();
	transform.q = newRotation;
	actor_->setGlobalPose(transform);

	//camera_->setPlayerFacingVector(getGlmFacingVector());
}

float Player::getFacingAngle() {
	return facingAngle_;
}

PxVec3 Player::getFacingVector() {
	return facingVector_;
}

glm::vec3 Player::getGlmFacingVector() {
	return glm::vec3(facingVector_.x, facingVector_.y, facingVector_.z);
}