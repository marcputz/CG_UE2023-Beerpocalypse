#include "NewGameObject.h"

NewGameObject::NewGameObject(string name, MyShader* shader, PxPhysics* physics, string modelPath, bool isStatic)
	: shader_{ shader },
	name_{ name },
	physics_{ physics },
	transform_{ new Transform(glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(1,1,1)) }
{
	model_ = new MyModel(modelPath);

	if (isStatic) {
		physicsActor_ = physics_->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));
	}
	else {
		physicsActor_ = physics_->createRigidDynamic(PxTransform(PxVec3(0, 0, 0)));
	}
	this->isStatic_ = isStatic;

	physicsActor_->userData = this;
}

void NewGameObject::synchronizeTransforms() {
	// Get the transform coordinates of the physics object and put them into the object's transform
	PxTransform physicsTransform = physicsActor_->getGlobalPose();
	transform_->setWorldPosition(asGlmVec3(physicsTransform.p));
	transform_->setWorldRotation(asGlmQuat(physicsTransform.q));
}

void NewGameObject::setCollider(PxShape* colliderShape) {
	physicsShape_ = colliderShape;

	if (physicsShape_ != nullptr) {
		// apply scale
		PxGeometryHolder geometryHolder = colliderShape->getGeometry();
		PxGeometry& geometry = geometryHolder.any();
		if (geometryHolder.any().getType() == PxGeometryType::eBOX) {
			PxBoxGeometry& boxGeometry = static_cast<PxBoxGeometry&>(geometry);
			boxGeometry.halfExtents.x *= getScale().x;
			boxGeometry.halfExtents.y *= getScale().y;
			boxGeometry.halfExtents.z *= getScale().z;
			colliderShape->setGeometry(boxGeometry);
		}

		physicsShape_->userData = this;
		physicsActor_->attachShape(*physicsShape_);
	}
}

void NewGameObject::setParent(NewGameObject* newParent) {
	if (newParent != nullptr) {
		this->transform_->setParent(newParent->transform_);
	}
	else {
		this->transform_->setParent(nullptr);
	}
}

void NewGameObject::setHealth(int newHealth) {
	int oldHealth = health;
	health = newHealth;

	if (health < 0) {
		health = 0;
	}
	if (health > maxHealth) {
		health = maxHealth;
	}

	onHealthChange(oldHealth, newHealth);
}

int NewGameObject::getHealth() {
	return health;
}

void NewGameObject::setLocalPosition(glm::vec3 newPosition) {
	this->transform_->setLocalPosition(newPosition);

	PxTransform transf = physicsActor_->getGlobalPose();
	transf.p = asPxVec3(this->transform_->getWorldPosition());
	physicsActor_->setGlobalPose(transf);
}

void NewGameObject::setLocalRotation(glm::quat newRotation) {
	this->transform_->setLocalRotation(newRotation);

	PxTransform transf = physicsActor_->getGlobalPose();
	transf.q = asPxQuat(this->transform_->getLocalRotation());
	physicsActor_->setGlobalPose(transf);
}

void NewGameObject::setScale(glm::vec3 newScale) {
	this->transform_->setScale(newScale);

	if (physicsShape_ != nullptr) {
		PxGeometryHolder geometryHolder = physicsShape_->getGeometry();
		PxGeometry& geometry = geometryHolder.any();
		if (geometry.getType() == PxGeometryType::eBOX) {
			PxBoxGeometry& boxGeometry = static_cast<PxBoxGeometry&>(geometry);
			boxGeometry.halfExtents.x *= getScale().x;
			boxGeometry.halfExtents.y *= getScale().y;
			boxGeometry.halfExtents.z *= getScale().z;

			physicsActor_->detachShape(*physicsShape_);
			physicsShape_->setGeometry(boxGeometry);
			physicsActor_->attachShape(*physicsShape_);
		}
	}
}

glm::vec3 NewGameObject::getLocalPosition() {
	return this->transform_->getLocalPosition();
}

glm::quat NewGameObject::getLocalRotation() {
	return this->transform_->getLocalRotation();
}

glm::vec3 NewGameObject::getWorldPosition() {
	return this->transform_->getWorldPosition();
}

glm::quat NewGameObject::getWorldRotation() {
	return this->transform_->getWorldRotation();
}

glm::vec3 NewGameObject::getScale() {
	return this->transform_->getScale();
}

glm::vec3 NewGameObject::getForwardVector() {
	return this->transform_->getForwardVector();
}

MyModel* NewGameObject::getModel() {
	return this->model_;
}

void NewGameObject::setAnimator(MyAnimator& newAnimator) {
	this->animator_ = &newAnimator;
}

/**
* This update function must be called BEFORE the physics update
*/
void NewGameObject::beforeUpdate() {
	// Call custom code
	onBeforeUpdate();
}

/**
* This update function must be called AFTER the physics update
*/
void NewGameObject::update(float deltaTime) {
	// synchronize physcis transform into object's transform
	synchronizeTransforms();

	if (this->animator_ != nullptr && isVisible_) {
		this->animator_->updateAnimation(deltaTime);
	}

	// Call custom code
	onUpdate(deltaTime);
}

//virtual void handleKeyboardInput(GLFWwindow* window, float deltaTime) = 0;
//virtual void handleMouseInput(float xOffset, float yOffset) = 0;

void NewGameObject::draw() {
	if (!isVisible_) {
		return;
	}

	glm::vec3 position = transform_->getWorldPosition();
	// TODO: Replace with world rotation and check if it's working
	glm::quat rotation = transform_->getLocalRotation();
	glm::vec3 scale = transform_->getScale();

	glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotMatrix = glm::mat4_cast(rotation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = transMatrix * rotMatrix * scaleMatrix;

	shader_->use();

	if (this->animator_ != nullptr) {
		std::vector<glm::mat4> finalBoneTransforms = animator_->getFinalBoneMatrices();

		for (int i = 0; i < finalBoneTransforms.size(); i++) {
			shader_->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", finalBoneTransforms[i]);
		}
	}

	shader_->setMat4("model", modelMatrix);
	model_->draw(*shader_);
}

glm::vec3 NewGameObject::asGlmVec3(physx::PxVec3 vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

physx::PxVec3 NewGameObject::asPxVec3(glm::vec3 vec) {
	return physx::PxVec3(vec.x, vec.y, vec.z);
}

glm::quat NewGameObject::asGlmQuat(physx::PxQuat quat) {
	return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

physx::PxQuat NewGameObject::asPxQuat(glm::quat quat) {
	return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
}