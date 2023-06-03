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

	// Lock rotation axis
	PxRigidDynamic* dyn = static_cast<PxRigidDynamic*>(physicsActor_);
	dyn->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);
}

void Zombie::onBeforeUpdate() {}

void Zombie::onUpdate(float deltaTime) {
	if (isVisible()) {
		if (isFollowing()) {
			glm::vec3 targetPosition = followObject->getWorldPosition();
			glm::vec3 zombiePosition = getWorldPosition();
			glm::vec3 direction = targetPosition - zombiePosition;
			direction.y = 0;
			direction = glm::normalize(direction);

			this->setWorldPosition(zombiePosition + (direction * deltaTime * movementSpeed));

			direction.x *= (-1.0f);
			glm::mat4 rotationMatrix = glm::lookAt(zombiePosition, zombiePosition - direction, glm::vec3(0, 1, 0));
			glm::quat rotation = glm::quat(rotationMatrix);

			this->setLocalRotation(rotation);
		}
	}
}

bool Zombie::isFollowing() {
	return followObject != nullptr;
}

NewGameObject* Zombie::getFollowing() {
	return followObject;
}

void Zombie::follow(NewGameObject* objectToFollow) {
	followObject = objectToFollow;
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
