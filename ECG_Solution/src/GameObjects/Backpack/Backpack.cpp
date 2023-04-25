#include "Backpack.h"

Backpack::Backpack(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo) : GameObject(shader, physics, goInfo) {
	
}

void Backpack::update(float deltaTime) {
	updateChildren(deltaTime);
}

void Backpack::handleInput(GLFWwindow* window, float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		//camera_->processKeyboardInput(FORWARD, deltaTime);

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		//camera_->processKeyboardInput(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		//camera_->processKeyboardInput(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		//camera_->processKeyboardInput(RIGHT, deltaTime);
	}
}