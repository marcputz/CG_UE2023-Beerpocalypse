#include "Zombie.h"

extern bool freezeZombies;

Zombie::Zombie(MyShader* shader, PxPhysics* physics) : GameObject("Zombie", shader, physics, "zombie/Zombie.dae", false) {
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

	highlightColor_ = glm::vec3(0.8f, 0.3, 0.3f);
	highlightDuration_ = 0.25f;
}

void Zombie::onBeforeUpdate() {}

void Zombie::onUpdate(float deltaTime) {
	if (isVisible()) {
		if (isFollowing() && !freezeZombies) {
			glm::vec3 targetPosition = followObject->getWorldPosition();
			glm::vec3 zombiePosition = getWorldPosition();
			glm::vec3 direction = targetPosition - zombiePosition;
			direction.y = 0;
			direction = glm::normalize(direction);

			// give targetPosition a little safety distance to avoid clipping into it completely
			const float safetyDistance = 0.8f;
			targetPosition = targetPosition - (direction * safetyDistance);
			glm::vec3 correctedDirection = targetPosition - zombiePosition;
			correctedDirection.y = 0;
			correctedDirection = glm::normalize(correctedDirection);

			this->setWorldPosition(zombiePosition + (correctedDirection * deltaTime * movementSpeed));

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

GameObject* Zombie::getFollowing() {
	return followObject;
}

void Zombie::follow(GameObject* objectToFollow) {
	followObject = objectToFollow;
}

void Zombie::playIdleAnim() {
	this->animator_->changeAnimation(Animation_Enum::IDLE);
}

void Zombie::playWalkingAnim() {
	this->animator_->changeAnimation(Animation_Enum::WALKING, 2.0f);
}

void Zombie::playAttackAnim() {
	this->animator_->changeAnimation(Animation_Enum::ATTACKING, 1.5f, true);
}


void Zombie::resetSpecifics() {
	//std::cout << "Reset Zombie" << std::endl;
}

void Zombie::processWindowInput(GLFWwindow* window, float deltaTime) {

}

void Zombie::processMouseInput(float offsetX, float offsetY) {

}

void Zombie::onHealthChange(int oldHealth, int newHealth) {
	if (newHealth < oldHealth) {
		// make grunt sound
		int rand = std::rand() % 6;
		string sound = "grunt_" + to_string(rand);
		MyAssetManager::playSound(sound);
	}

	if (newHealth <= 0) {
		setVisible(false);
		enableCollider(false);
	}
	this->highlighted_ = true;
	this->remainingHighlightDuration_ = this->highlightDuration_;
}
