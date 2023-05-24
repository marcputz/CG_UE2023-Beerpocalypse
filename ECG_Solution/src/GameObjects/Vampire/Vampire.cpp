#include "Vampire.h"

Vampire::Vampire(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo) : GameObject(shader, physics, goInfo) {

}

void Vampire::update(float deltaTime) {
	updateChildren(deltaTime);
}

void Vampire::handleKeyboardInput(GLFWwindow* window, float deltaTime) {

}

void Vampire::handleMouseInput(float xOffset, float yOffset) {

}