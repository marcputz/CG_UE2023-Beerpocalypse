#include "Zombie.h"

Zombie::Zombie(MyShader* shader, PxPhysics* physics) : NewGameObject("Zombie", shader, physics, "zombie/Zombie.dae", false) {
	PxMaterial* material = physics->createMaterial(1, 1, 0.01f);
	PxBoxGeometry geometry = PxBoxGeometry(0.3, 0.7, 0.3);
	PxShape* collider = physics->createShape(geometry, *material);
	PxFilterData collissionFilterData;
	collissionFilterData.word0 = CollisionLayer::LAYER_ENEMIES; // Own ID
	collissionFilterData.word1 = CollisionLayer::LAYER_PLAYER; // IDs to do collision callback with
	collider->setSimulationFilterData(collissionFilterData);

	collider->setLocalPose(PxTransform(PxVec3(0, 0.7, 0)));

	setCollider(collider);
}

void Zombie::onBeforeUpdate() {}

void Zombie::onUpdate(float deltaTime) {

}

void Zombie::processWindowInput(GLFWwindow* window, float deltaTime) {

}

void Zombie::processMouseInput(float offsetX, float offsetY) {

}

void Zombie::onHealthChange(int oldHealth, int newHealth)
{
	if (newHealth <= 0) {
		setVisible(false);
	}
}
