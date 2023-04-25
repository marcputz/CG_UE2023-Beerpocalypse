#include "GameObject.h"
#include "PxPhysicsAPI.h"

using namespace physx;
using namespace std;

GameObject::GameObject(const std::string& path, MyShader& shader) {
	model_ = new MyModel(path);
	shader_ = &shader;
}

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

PxRigidActor* GameObject::getActor() {
	return actor_;
}

void GameObject::setPosition(glm::vec3 pos) {
	setPosition(pos.x, pos.y, pos.z);
}

void GameObject::setPosition(physx::PxVec3 pos) {
	setPosition(pos.x, pos.y, pos.z);
}

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

