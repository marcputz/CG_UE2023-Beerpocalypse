#include "Zombie.h"

Zombie::Zombie(MyShader* shader, PxPhysics* physics) : NewGameObject("Zombie", shader, physics, "zombie/Zombie.dae", glm::vec3(0.5, 0.5, 0.5), true) {

}

void Zombie::damage(float damagePoints) {
	healthPoints -= damagePoints;
	
	if (healthPoints <= 0.0f) {
		// zombie was defeated
		this->setVisible(false);
	}
}

void Zombie::onBeforeUpdate() {}

void Zombie::onUpdate(float deltaTime) {

}

void Zombie::processWindowInput(GLFWwindow* window, float deltaTime) {

}

void Zombie::processMouseInput(float offsetX, float offsetY) {

}