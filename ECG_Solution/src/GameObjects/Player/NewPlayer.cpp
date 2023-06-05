#include "NewPlayer.h"

extern int score;

NewPlayer::NewPlayer(MyShader* shader, PxPhysics* physics) : NewGameObject("Player", shader, physics, "cube/brick_cube/cube.obj", false) {
	
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

void NewPlayer::onBeforeUpdate() {

}

void NewPlayer::onUpdate(float deltaTime) {
	if (damageCooldown > 0.01f) {
		damageCooldown -= deltaTime;
	}

	glm::vec3 pos = this->getWorldPosition();
	if (pos.y < -5.0) {
		this->setWorldPosition(this->respawnPoint_);
	}

	// check if player is on the ground
	/*PxVec3 groundRayDirection = PxVec3(0, -1, 0);
	PxVec3 groundRayOrigin = asPxVec3(getWorldPosition());
	const PxU32 hitBufferSize = 2;
	PxRaycastHit hitBuffer[hitBufferSize];
	PxRaycastBuffer buf(hitBuffer, hitBufferSize);
	bool raycastStatus = scene->getPhysicsScene()->raycast(groundRayOrigin, groundRayDirection, 1.1f, buf);
	if (raycastStatus) {
		// Raycast has hit something
		for (PxU32 i = 0; i < buf.nbTouches; i++) {
			NewGameObject* object = static_cast<NewGameObject*>(buf.touches[i].actor->userData);
			std::cout << object->name_ << std::endl;
		}
	}
	else {
		onGround = false;
	}*/
}

void NewPlayer::onCollision(NewGameObject* otherObject) {
	//std::cout << "Player collided with '" << otherObject->name_ << "'" << std::endl;

	Beer* beer = dynamic_cast<Beer*>(otherObject);
	if (beer != nullptr) {
		// Collided with beer
		if (beer->isVisible()) {
			this->setHealth(this->getHealth() + 20);
			score++;
			beer->setVisible(false);
		}
		return;
	}

	Zombie* zombie = dynamic_cast<Zombie*>(otherObject);
	if (zombie != nullptr) {
		// Collided with zombie
		if (zombie->isVisible()) {
			if (damageCooldown <= 0.01f) {
				setHealth(getHealth() - 10);
				damageCooldown = targetDamageCooldown;
			}
		}
	}
}

void NewPlayer::swapCamera() {
	if (activeCameraType == PlayerCameraType::CAMERA_FIRST_PERSON) {
		activeCameraType = PlayerCameraType::CAMERA_THIRD_PERSON;
		setVisible(true);
	}
	else {
		activeCameraType = PlayerCameraType::CAMERA_FIRST_PERSON;
		setVisible(false);
	}
}

bool NewPlayer::isOnGround() {
	return this->onGround;
}

void NewPlayer::setIsOnGround(bool newValue) {
	this->onGround = newValue;
}

PlayerCamera* NewPlayer::getActiveCamera() {
	switch (activeCameraType) {
	case PlayerCameraType::CAMERA_FIRST_PERSON:
		return firstPersonCamera;
	case PlayerCameraType::CAMERA_THIRD_PERSON:
		return thirdPersonCamera;
	}

	return thirdPersonCamera;
}

PlayerCameraType NewPlayer::getActiveCameraType() {
	return activeCameraType;
}

void NewPlayer::processWindowInput(GLFWwindow* window, float deltaTime) {
	glm::vec3 forward = getForwardVector();
	glm::vec3 backward = -forward;
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
	glm::vec3 left = -right;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += forward * (movementSpeed * deltaTime);
		setLocalPosition(position);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += backward * (movementSpeed * deltaTime);
		setLocalPosition(position);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += left * (movementSpeed * deltaTime);
		setLocalPosition(position);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 position = getLocalPosition();
		position += right * (movementSpeed * deltaTime);
		setLocalPosition(position);
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

void NewPlayer::processMouseInput(float offsetX, float offsetY) {
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

void NewPlayer::onHealthChange(int oldHealth, int newHealth)
{
}

void NewPlayer::setRespawnPoint(glm::vec3& newRespawnPoint) {
	this->respawnPoint_ = newRespawnPoint;
}
