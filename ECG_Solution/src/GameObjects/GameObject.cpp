#include "GameObject.h"



// Constructor

GameObject::GameObject(MyShader& shader, PxPhysics* physics, GameObjectInfo& goInfo) {
	if (goInfo.modelPath != "") {
		model_ = new MyModel(goInfo.modelPath);
		shader_ = &shader;

		actorType_ = goInfo.actorType;

		PxMaterial* material = physics->createMaterial(goInfo.staticFriction, goInfo.dynamicFriction, goInfo.restitution);
		//PxShape* shape = physics->createShape(PxBoxGeometry(1, 1, 1), *material);
		PxShape* shape = physics->createShape(PxBoxGeometry(model_->boundingBox_.absDiff.x / 2.0f, model_->boundingBox_.absDiff.y / 2.0f, model_->boundingBox_.absDiff.z / 2.0f), *material);

		switch (goInfo.actorType) {
		case GameObjectActorType::TYPE_DYNAMIC:
			actor_ = physics->createRigidDynamic(PxTransform(goInfo.location));
			break;
		default:
			actor_ = physics->createRigidStatic(PxTransform(goInfo.location));
			break;
		}
		actor_->attachShape(*shape);
	}
}

// OpenGL Methods

glm::mat4 pxMat44ToGlmMat4(PxMat44 mat) {
	return glm::mat4{
		mat.column0.x, mat.column0.y, mat.column0.z, mat.column0.w,
		mat.column1.x, mat.column1.y, mat.column1.z, mat.column1.w,
		mat.column2.x, mat.column2.y, mat.column2.z, mat.column2.w,
		mat.column3.x, mat.column3.y, mat.column3.z, mat.column3.w,
	};
}

void GameObject::draw() {
	PxTransform transform = actor_->getGlobalPose();
	PxMat44 pxMat = PxMat44(transform);
	glm::mat4 modelMatrix = pxMat44ToGlmMat4(pxMat);

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
	PxTransform newTrans(x, y, z);
	actor_->setGlobalPose(newTrans);
}

PxVec3 GameObject::getPosition() {
	return actor_->getGlobalPose().p;
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

