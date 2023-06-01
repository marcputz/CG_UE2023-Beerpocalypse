#include "Transform.h"

Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
	this->localPosition = position;
	this->localRotation = rotation;
	this->scale = scale;
}

void Transform::setParent(Transform* parentTransform) {
	this->parent = parentTransform;
}

void Transform::setLocalPosition(glm::vec3 newPosition) {
	this->localPosition = newPosition;
}

void Transform::setLocalRotation(glm::quat newRotation) {
	this->localRotation = newRotation;
}

void Transform::setScale(glm::vec3 newScale) {
	this->scale = newScale;
}

glm::vec3 Transform::getLocalPosition() {
	return localPosition;
}

glm::quat Transform::getLocalRotation() {
	return localRotation;
}

glm::vec3 Transform::getScale() {
	return scale;
}

void Transform::setWorldPosition(glm::vec3 newPosition) {
	if (parent != nullptr) {
		localPosition = newPosition - parent->getWorldPosition();
	}
	else {
		localPosition = newPosition;
	}
}

void Transform::setWorldRotation(glm::quat newRotation) {
	if (parent != nullptr) {
		localRotation = newRotation * glm::inverse(parent->getWorldRotation());
	}
	else {
		localRotation = newRotation;
	}
}

glm::vec3 Transform::getWorldPosition() {
	if (parent != nullptr) {
		return parent->getWorldPosition() + localPosition;
	}
	else {
		return localPosition;
	}
}
glm::quat Transform::getWorldRotation() {
	if (parent != nullptr) {
		return localRotation * parent->getWorldRotation();
	}
	else {
		return localRotation;
	}
}

glm::vec3 Transform::getForwardVector() {
	glm::vec3 forward = glm::vec3(0, 0, 1) * localRotation;
	forward.x *= -1;
	return forward;
}