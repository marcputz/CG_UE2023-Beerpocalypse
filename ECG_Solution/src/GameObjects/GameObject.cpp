#include "GameObject.h"


GameObject::GameObject(const std::string& path) {
	model_ = new MyModel(path);
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

void GameObject::draw(MyShader& shader) {
	shader.use();
	if (transform_.isDirty()) {
		transform_.computeModelMatrix();
	}
	shader.setMat4("model", transform_.getModelMatrix());
	model_->draw(shader);

	for (auto&& child : children_) {
		child->draw(shader);
	}
}

void GameObject::addChild(const std::string& path) {
	children_.emplace_back(std::make_unique<GameObject>(path));
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

