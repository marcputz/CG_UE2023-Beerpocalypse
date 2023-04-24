#include "MyTransform.h"

void MyTransform::computeModelMatrix() {
	model_ = getLocalModelMatrix();
	dirty_ = false;
}

void MyTransform::computeModelMatrix(const glm::mat4& parentGlobalModelMatrix) {
	model_ = parentGlobalModelMatrix * getLocalModelMatrix();
	dirty_ = false;
}

const glm::mat4& MyTransform::getModelMatrix() {
	return model_;
}

void MyTransform::setLocalPosition(const glm::vec3& newPosition) {
	position_ = newPosition;
	dirty_ = true;
}

const glm::vec3& MyTransform::getLocalPosition() {
	return position_;
}

void MyTransform::setRotation(const glm::vec3& newRotation) {
	rotation_ = newRotation;
	dirty_ = true;
}

const glm::vec3& MyTransform::getRotation() {
	return rotation_;
}

void MyTransform::setScale(const glm::vec3& newScale) {
	scale_ = newScale;
	dirty_ = true;
}

const glm::vec3& MyTransform::getScale() {
	return scale_;
}

bool MyTransform::isDirty() {
	return dirty_;
}

glm::mat4 MyTransform::getLocalModelMatrix() {
	glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_.x), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_.z), glm::vec3(0.0f, 0.0f, 1.0f));

	const glm::mat4 rotationMatrix = rotY * rotX * rotZ;

	// translation * rotation * scale
	return glm::translate(glm::mat4(1.0f), position_) * rotationMatrix * glm::scale(glm::mat4(1.0f), scale_);
}
