#include "GameObject.h"

GameObject::GameObject(string name, MyShader* shader, PxPhysics* physics, string modelPath, bool isStatic)
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

void GameObject::synchronizeTransforms() {
	// Get the transform coordinates of the physics object and put them into the object's transform
	PxTransform physicsTransform = physicsActor_->getGlobalPose();
	transform_->setWorldPosition(asGlmVec3(physicsTransform.p));
	transform_->setWorldRotation(asGlmQuat(physicsTransform.q));
}

void GameObject::setCollider(PxGeometry* geometry, PxMaterial* material) {
	physicsMaterial_ = material;

	if (geometry != nullptr && material != nullptr) {
		// create shape and attach (automatically)
		physicsShape_ = PxRigidActorExt::createExclusiveShape(*(this->getRigidActor()), *geometry, *material);
		physicsShape_->userData = this;

		// apply scale to collider
		if (physicsShape_ != nullptr) {
			if (geometry->getType() == PxGeometryType::eBOX) {
				PxBoxGeometry* boxGeometry = static_cast<PxBoxGeometry*>(geometry);
				boxGeometry->halfExtents.x *= getScale().x;
				boxGeometry->halfExtents.y *= getScale().y;
				boxGeometry->halfExtents.z *= getScale().z;
			}
		}
	}
	else {
		if (physicsShape_ != nullptr)
			this->getRigidActor()->detachShape(*physicsShape_);
	}
}

void GameObject::enableCollider(bool enabled) {
	if (enabled) {
		if (physicsShape_ != nullptr) {
			physicsShape_->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
			physicsShape_->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		}
	}
	else {
		if (physicsShape_ != nullptr) {
			physicsShape_->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
			physicsShape_->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		}
	}
}

void GameObject::reset() {
	this->setLocalPosition(this->originalPosition);
	this->setHealth(100);
	this->setVisible(true);
	this->enableCollider(true);

	this->resetSpecifics();
}

bool GameObject::isInsideFrustum(const Frustum& frustum) {
	return true;
}

void GameObject::setParent(GameObject* newParent) {
	if (newParent != nullptr) {
		this->transform_->setParent(newParent->transform_);
	}
	else {
		this->transform_->setParent(nullptr);
	}
}

void GameObject::setHealth(int newHealth) {
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

int GameObject::getHealth() {
	return health;
}

void GameObject::setLocalPosition(glm::vec3 newPosition) {
	if (this->hasOriginalPosition == false) {
		this->originalPosition = newPosition;
		this->hasOriginalPosition = true;
	}

	this->transform_->setLocalPosition(newPosition);

	PxTransform transf = physicsActor_->getGlobalPose();
	transf.p = asPxVec3(this->transform_->getWorldPosition());
	physicsActor_->setGlobalPose(transf);
}

void GameObject::setLocalRotation(glm::quat newRotation) {
	this->transform_->setLocalRotation(newRotation);

	PxTransform transf = physicsActor_->getGlobalPose();
	transf.q = asPxQuat(this->transform_->getLocalRotation());
	physicsActor_->setGlobalPose(transf);
}

void GameObject::setWorldPosition(glm::vec3 newPosition) {
	this->transform_->setWorldPosition(newPosition);

	PxTransform transf = physicsActor_->getGlobalPose();
	transf.p = asPxVec3(this->transform_->getWorldPosition());
	physicsActor_->setGlobalPose(transf);
}

void GameObject::setScale(glm::vec3 newScale, bool applyTilingScale) {
	PxVec3 oldScale = asPxVec3(this->transform_->getScale());
	this->transform_->setScale(newScale);

	if (physicsShape_ != nullptr) {
		PxGeometryHolder geometryHolder = physicsShape_->getGeometry();
		PxGeometry& geometry = geometryHolder.any();
		if (geometry.getType() == PxGeometryType::eBOX) {
			PxBoxGeometry& boxGeometry = static_cast<PxBoxGeometry&>(geometry);
			boxGeometry.halfExtents.x /= oldScale.x;
			boxGeometry.halfExtents.y /= oldScale.y;
			boxGeometry.halfExtents.z /= oldScale.z;
			boxGeometry.halfExtents.x *= getScale().x;
			boxGeometry.halfExtents.y *= getScale().y;
			boxGeometry.halfExtents.z *= getScale().z;

			physicsShape_->setGeometry(boxGeometry);
		}
	}

	if (applyTilingScale == true) {
		this->model_->applyTilingScale(newScale.x, newScale.y, newScale.z);
	}
}

glm::vec3 GameObject::getLocalPosition() {
	return this->transform_->getLocalPosition();
}

glm::quat GameObject::getLocalRotation() {
	return this->transform_->getLocalRotation();
}

glm::vec3 GameObject::getWorldPosition() {
	return this->transform_->getWorldPosition();
}

glm::quat GameObject::getWorldRotation() {
	return this->transform_->getWorldRotation();
}

glm::vec3 GameObject::getScale() {
	return this->transform_->getScale();
}

glm::vec3 GameObject::getForwardVector() {
	return this->transform_->getForwardVector();
}

MyModel* GameObject::getModel() {
	return this->model_;
}

void GameObject::setAnimator(MyAnimator& newAnimator) {
	this->animator_ = &newAnimator;
	this->animator_->changeAnimation(Animation_Enum::IDLE);
}

/**
* This update function must be called BEFORE the physics update
*/
void GameObject::beforeUpdate() {
	// Call custom code
	onBeforeUpdate();
}

/**
* This update function must be called AFTER the physics update
*/
void GameObject::update(float deltaTime) {
	// synchronize physcis transform into object's transform
	synchronizeTransforms();

	if (this->animator_ != nullptr && isVisible_) {
		this->animator_->updateAnimation(deltaTime);
	}

	if (this->highlighted_) {
		this->remainingHighlightDuration_ -= deltaTime;
		if (this->remainingHighlightDuration_ <= 0.0f) {
			this->highlighted_ = false;
			this->remainingHighlightDuration_ = 0.0f;
		}
	}

	// Call custom code
	onUpdate(deltaTime);
}

//virtual void handleKeyboardInput(GLFWwindow* window, float deltaTime) = 0;
//virtual void handleMouseInput(float xOffset, float yOffset) = 0;

void GameObject::draw() {
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

		for (unsigned int i = 0; i < finalBoneTransforms.size(); i++) {
			shader_->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", finalBoneTransforms[i]);
		}
	}

	shader_->setInt("isHighlighted", this->highlighted_);
	shader_->setVec3("highlightColor", this->highlightColor_);

	shader_->setMat4("model", modelMatrix);
	model_->draw(*shader_);
}

glm::vec3 GameObject::asGlmVec3(physx::PxVec3 vec) {
	return glm::vec3(vec.x, vec.y, vec.z);
}

physx::PxVec3 GameObject::asPxVec3(glm::vec3 vec) {
	return physx::PxVec3(vec.x, vec.y, vec.z);
}

glm::quat GameObject::asGlmQuat(physx::PxQuat quat) {
	return glm::quat(quat.w, quat.x, quat.y, quat.z);
}

physx::PxQuat GameObject::asPxQuat(glm::quat quat) {
	return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
}