#include "BrickCube.h"

BrickCube::BrickCube(MyShader& shader, PxPhysics* physics, GameObjectInfo goInfo) : GameObject(shader, physics, goInfo) {

}

void BrickCube::update(float deltaTime) {
	updateChildren(deltaTime);
}

void BrickCube::handleInput(GLFWwindow* window, float deltaTime) {
	
}


