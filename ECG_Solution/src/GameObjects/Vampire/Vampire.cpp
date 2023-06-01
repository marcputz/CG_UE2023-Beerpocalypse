#include "Vampire.h"

Vampire::Vampire(MyShader* shader, PxPhysics* physics) : NewGameObject(shader, physics, "vampire/Vampire.dae", glm::vec3(0.5, 0.5, 0.5), true) {

}

void Vampire::onBeforeUpdate() {}

void Vampire::onUpdate(float deltaTime) {
	
}

void Vampire::processWindowInput(GLFWwindow* window, float deltaTime) {

}

void Vampire::processMouseInput(float offsetX, float offsetY) {

}