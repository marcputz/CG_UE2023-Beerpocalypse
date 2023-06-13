#include "Player.h"

extern int score;

Player::Player(MyShader* shader, PxPhysics* physics) : GameObject("Player", shader, physics, "cube/brick_cube/cube.obj", false) {
	
	PxMaterial* material = physics->createMaterial(0.5, 0.5, 0.0);
	PxBoxGeometry geometry = PxBoxGeometry(1, 1, 1);
	PxShape* collider = physics->createShape(geometry, *material);
	PxFilterData filterData;
	filterData.word0 = CollisionLayer::LAYER_PLAYER; // own ID
	filterData.word1 = CollisionLayer::LAYER_COLLECTABLES | CollisionLayer::LAYER_ENEMIES; // IDs to do collision callback with
	collider->setSimulationFilterData(filterData);

	setCollider(collider);
	
	thirdPersonCamera = new PlayerCameraThirdPerson(this->transform_);
	firstPersonCamera = new PlayerCameraFirstPerson(this->transform_);

	if (activeCameraType == PlayerCameraType::CAMERA_FIRST_PERSON) {
		setVisible(false);
	}
	else {
		setVisible(true);
	}

	// Lock rotation axis
	PxRigidDynamic* dyn = static_cast<PxRigidDynamic*>(physicsActor_);
	dyn->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);
}

void Player::onBeforeUpdate() {

}

void Player::onUpdate(float deltaTime) {
	if (damageCooldown > 0.01f) {
		damageCooldown -= deltaTime;
	}

	glm::vec3 pos = this->getWorldPosition();
	if (pos.y < -5.0) {
		this->setWorldPosition(this->respawnPoint_);
	}

	// footstep sounds
	if (playFootstepSound) {
		if (footstepTimeCounter <= 0) {
			MyAssetManager::playSound("footstep_0");
			footstepTimeCounter = FOOTSTEP_SOUND_DELAY;
		}
		else {
			footstepTimeCounter -= deltaTime;
		}
	}
	else {
		// play footstep faster when player start walking again
		footstepTimeCounter = FOOTSTEP_SOUND_DELAY / 8.0f;
	}
}

void Player::onCollision(GameObject* otherObject) {
	//std::cout << "Player collided with '" << otherObject->name_ << "'" << std::endl;

	Beer* beer = dynamic_cast<Beer*>(otherObject);
	if (beer != nullptr) {
		// Collided with beer
		if (beer->isVisible()) {
			this->setHealth(this->getHealth() + 20);
			score++;
			beer->setVisible(false);
			beer->enableCollider(false);
			MyAssetManager::playSound("bell");
		}
		return;
	}

	Zombie* zombie = dynamic_cast<Zombie*>(otherObject);
	if (zombie != nullptr) {
		// Collided with zombie
		if (zombie->isVisible()) {
			zombie->playAttackAnim();
			if (damageCooldown <= 0.01f) {
				setHealth(getHealth() - 10);
				damageCooldown = targetDamageCooldown;
			}
		}
		return;
	}
}

void Player::swapCamera() {
	if (activeCameraType == PlayerCameraType::CAMERA_FIRST_PERSON) {
		activeCameraType = PlayerCameraType::CAMERA_THIRD_PERSON;
		setVisible(true);
	}
	else {
		activeCameraType = PlayerCameraType::CAMERA_FIRST_PERSON;
		setVisible(false);
	}
}

bool Player::isOnGround() {
	return this->onGround;
}

void Player::setIsOnGround(bool newValue) {
	this->onGround = newValue;
}

PlayerCamera* Player::getActiveCamera() {
	switch (activeCameraType) {
	case PlayerCameraType::CAMERA_FIRST_PERSON:
		return firstPersonCamera;
	case PlayerCameraType::CAMERA_THIRD_PERSON:
		return thirdPersonCamera;
	}

	return thirdPersonCamera;
}

PlayerCameraType Player::getActiveCameraType() {
	return activeCameraType;
}

void Player::processWindowInput(GLFWwindow* window, float deltaTime) {
	glm::vec3 forward = getForwardVector();
	glm::vec3 backward = -forward;
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
	glm::vec3 left = -right;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += forward * (movementSpeed * deltaTime);
		setLocalPosition(position);
		playFootstepSound = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += backward * (movementSpeed * deltaTime);
		setLocalPosition(position);
		playFootstepSound = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += left * (movementSpeed * deltaTime);
		setLocalPosition(position);
		playFootstepSound = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += right * (movementSpeed * deltaTime);
		setLocalPosition(position);
		playFootstepSound = true;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE) {
		playFootstepSound = false;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		// jump
		if (onGround) {
			PxRigidDynamic* dyn = static_cast<PxRigidDynamic*>(physicsActor_);
			dyn->addForce(PxVec3(0, 1, 0) * jumpForce, PxForceMode::eIMPULSE);
			//dyn->setForceAndTorque(PxVec3(0, 1, 0) * jumpForce, PxVec3(0, 0, 0), PxForceMode::eIMPULSE);
			this->onGround = false;
		}
	}
}

void Player::processMouseInput(float offsetX, float offsetY) {
	firstPersonCamera->processMouseInput(offsetX, offsetY);
	thirdPersonCamera->processMouseInput(offsetX, offsetY);

	facingAngle += -offsetX / 200.0f * turningSpeed;

	if (facingAngle < 0) {
		facingAngle += 360;
	}
	if (facingAngle > 360) {
		facingAngle -= 360;
	}

	glm::vec3 rotationAxis{ 0, 1, 0 };
	glm::quat newRotation = asGlmQuat(PxQuat(glm::radians(facingAngle), asPxVec3(rotationAxis).getNormalized()));
	setLocalRotation(newRotation);
}

void Player::processMouseScrolling(double yOffset) {
	float fov = this->firstPersonCamera->getFov();
	fov -= (float)yOffset;
	if (fov < 20.0f) {
		fov = 20.0f;
	}
	if (fov > 90.0f) {
		fov = 90.0f;
	}
	this->firstPersonCamera->setFov(fov);
	this->thirdPersonCamera->setFov(fov);
}

void Player::onHealthChange(int oldHealth, int newHealth){
	if (newHealth < oldHealth) {
		// play sound
		int rand = std::rand() % 7;
		string sound = "zombie_attack_" + to_string(rand);
		MyAssetManager::playSound(sound);
	}
}

void Player::setRespawnPoint(glm::vec3& newRespawnPoint) {
	this->respawnPoint_ = newRespawnPoint;
}

void Player::setCameraFOVs(float newFov) {
	firstPersonCamera->setFov(newFov);
	thirdPersonCamera->setFov(newFov);
}

void Player::resetSpecifics() {
	//std::cout << "Reset Player" << std::endl;
	if (activeCameraType == CAMERA_FIRST_PERSON) {
		setVisible(false);
	}
}
