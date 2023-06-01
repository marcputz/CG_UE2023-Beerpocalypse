#include "NewPlayer.h"

NewPlayer::NewPlayer(MyShader* shader, PxPhysics* physics) : NewGameObject(shader, physics, "cube/brick_cube/cube.obj", glm::vec3(0.5, 0.5, 0.5), true) {

}

void NewPlayer::onBeforeUpdate() {

}

void NewPlayer::onUpdate(float deltaTime) {

}

void NewPlayer::processWindowInput(GLFWwindow* window, float deltaTime) {
	glm::vec3 forward = getForwardVector();
	glm::vec3 backward = -forward;
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
	glm::vec3 left = -right;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += forward * (movementSpeed * deltaTime);
		setLocalPosition(position);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += backward * (movementSpeed * deltaTime);
		setLocalPosition(position);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += left * (movementSpeed * deltaTime);
		setLocalPosition(position);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += right * (movementSpeed * deltaTime);
		setLocalPosition(position);
	}
}

void NewPlayer::processMouseInput(float offsetX, float offsetY) {
	facingAngle += -offsetX / 200.0f * turningSpeed;

	if (facingAngle < 0) {
		facingAngle += 360;
	}
	if (facingAngle > 360) {
		facingAngle -= 360;
	}

	glm::vec3 rotationAxis{ 0, 1, 0 };
	glm::quat newRotation = asGlmQuat(PxQuat(glm::radians(facingAngle), asPxVec3(rotationAxis).getNormalized()));
	setLocalRotation(newRotation);
}