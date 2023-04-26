#include "Cube.h"

Cube::Cube(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo) : GameObject(shader, physics, goInfo) {

}

void Cube::update(float deltaTime) {
	updateChildren(deltaTime);
}

void Cube::handleInput(GLFWwindow* window, float deltaTime) {
	
}


