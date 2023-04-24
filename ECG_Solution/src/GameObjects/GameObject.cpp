#include "GameObject.h"


GameObject::GameObject(const std::string& path, MyShader& shader) {
	model_ = new MyModel(path);
	shader_ = &shader;
}

GameObject::~GameObject() {

}

void GameObject::update(float deltaTime) {
	transform_.setRotation(transform_.getRotation() + glm::vec3(0.0f, 45.0f * deltaTime, 0.0f));

	if (parent_) {
		transform_.computeModelMatrix(parent_->transform_.getModelMatrix());
	}
	else {
		transform_.computeModelMatrix();
	}

	for (auto&& child : children_) {
		child->update(deltaTime);
	}
}

void GameObject::draw() {
	shader_->use();
	if (transform_.isDirty()) {
		transform_.computeModelMatrix();
	}
	shader_->setMat4("model", transform_.getModelMatrix());
	model_->draw(*shader_);

	for (auto&& child : children_) {
		child->draw();
	}
}

void GameObject::addChild(const std::string& path, MyShader& shader) {
	children_.emplace_back(std::make_unique<GameObject>(path, shader));
	children_.back()->parent_ = this;
}

void GameObject::addChild(std::shared_ptr<GameObject> child) {
	children_.emplace_back(std::move(child));
	children_.back()->parent_ = this;
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

