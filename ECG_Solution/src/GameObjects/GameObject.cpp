#include "GameObject.h"

// Constructor

GameObject::GameObject(MyShader& shader, PxPhysics* physics, GameObjectInfo& goInfo) {
	if (goInfo.modelPath != "") {
		model_ = new MyModel(goInfo.modelPath);
		shader_ = &shader;
		scale_ = goInfo.scale;

		PxMaterial* material = physics->createMaterial(goInfo.staticFriction, goInfo.dynamicFriction, goInfo.restitution);
		//PxShape* shape = physics->createShape(PxBoxGeometry(1, 1, 1), *material);
		float boundingBoxX = model_->boundingBox_.absDiff.x / 2.0f;
		float boundingBoxY = model_->boundingBox_.absDiff.y / 2.0f;
		float boundingBoxZ = model_->boundingBox_.absDiff.z / 2.0f;
		PxShape* shape = physics->createShape(PxBoxGeometry(boundingBoxX * scale_.x, boundingBoxY * scale_.y, boundingBoxZ * scale_.z), *material);

		switch (goInfo.actorType) {
			case GameObjectActorType::TYPE_DYNAMIC:
				actor_ = physics->createRigidDynamic(PxTransform(goInfo.location));
				actorType_ = TYPE_DYNAMIC;
				break;
			default:
				actor_ = physics->createRigidStatic(PxTransform(goInfo.location));
				actorType_ = TYPE_STATIC;
				break;
		}
		actor_->attachShape(*shape);
	}
}

// OpenGL Methods

void GameObject::draw() {
	PxTransform transform = actor_->getGlobalPose();
	glm::vec3 translate = asGlmVec3(transform.p);
	glm::quat rotation = asGlmQuat(transform.q);
	glm::vec3 scale = asGlmVec3(scale_);

	glm::mat4 transMatrix = glm::translate(glm::mat4(1.0f), translate);
	glm::mat4 rotMatrix = glm::mat4_cast(rotation);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = transMatrix * rotMatrix * scaleMatrix;

	//PxMat44 pxMat = PxMat44(transform);
	//glm::mat4 modelMatrix = pxMat44ToGlmMat4(pxMat);

	shader_->use();
	shader_->setMat4("model", modelMatrix);
	model_->draw(*shader_);

	for (auto&& child : children_) {
		child->draw();
	}
}

// Physics

PxRigidActor* GameObject::getActor() {
	return actor_;
}

// Transform-Methods

void GameObject::setPosition(glm::vec3 pos) {
	setPosition(pos.x, pos.y, pos.z);
}

void GameObject::setPosition(physx::PxVec3 pos) {
	setPosition(pos.x, pos.y, pos.z);
}

void GameObject::setPosition(float x, float y, float z) {
	PxTransform trans = actor_->getGlobalPose();
	trans.p = PxVec3(x, y, z);
	actor_->setGlobalPose(trans);
}

PxVec3 GameObject::getPosition() {
	return actor_->getGlobalPose().p;
}

glm::vec3 GameObject::getGlmPosition() {
	PxVec3 pos = getPosition();
	return glm::vec3(pos.x, pos.y, pos.z);
}

MyModel* GameObject::getModel() {
	return model_;
}

void GameObject::move(physx::PxVec3 moveVector) {
	if (actorType_ == TYPE_DYNAMIC) {

		PxVec3 pos = this->getPosition();
		pos = pos + moveVector;
		this->setPosition(pos);

		//PxRigidDynamic* rigidDynamic = static_cast<PxRigidDynamic*>(actor_);
		//rigidDynamic->setLinearVelocity(direction);
	}
}

// Children-Parent Methods

void GameObject::addChild(GameObject* child) {
	children_.emplace_back(child);
	children_.back()->parent_ = this;
}

void GameObject::updateChildren(float deltaTime) {
	for (auto&& child : children_) {
		child->update(deltaTime);
	}
}

/*void GameObject::updateSelfAndChild() {
	if (transform_.isDirty()) {
		forceUpdateSelfAndChild();
		return;
	}

	for (auto&& child : children_) {
		child->updateSelfAndChild();
	}
}

void GameObject::forceUpdateSelfAndChild() {
	if (parent_) {
		transform_.computeModelMatrix(parent_->transform_.getModelMatrix());
	} else {
		transform_.computeModelMatrix();
	}

	for (auto&& child : children_) {
		child->forceUpdateSelfAndChild();
	}
}
*/

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