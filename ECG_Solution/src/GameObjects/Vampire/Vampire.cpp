#include "Vampire.h"

Vampire::Vampire(MyShader* shader, PxPhysics* physics) : NewGameObject("Vampire", shader, physics, "vampire/Vampire.dae", true) {

}

void Vampire::onBeforeUpdate() {}

void Vampire::onUpdate(float deltaTime) {
	
}

void Vampire::processWindowInput(GLFWwindow* window, float deltaTime) {

}

void Vampire::processMouseInput(float offsetX, float offsetY) {

}

void Vampire::onHealthChange(int oldHealth, int newHealth)
{
}

void Vampire::resetSpecifics() {
	//std::cout << "Reset Vampire" << std::endl;
}
