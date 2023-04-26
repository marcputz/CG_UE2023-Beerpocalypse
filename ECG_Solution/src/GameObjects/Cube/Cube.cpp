#include "Cube.h"

Cube::Cube(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo) : GameObject(shader, physics, goInfo) {

}

void Cube::update(float deltaTime) {
	updateChildren(deltaTime);
}

void Cube::handleKeyboardInput(GLFWwindow* window, float deltaTime) {
	
}

void Cube::handleMouseInput(float xOffset, float yOffset) {

}

