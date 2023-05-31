#include "Transform.h"

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
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

void Transform::setLocalRotation(glm::vec3 newRotation) {
	this->localRotation = newRotation;
}

void Transform::setScale(glm::vec3 newScale) {
	this->scale = newScale;
}

glm::vec3 Transform::getLocalPosition() {
	return localPosition;
}

glm::vec3 Transform::getLocalRotation() {
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

void Transform::setWorldRotation(glm::vec3 newRotation) {
	if (parent != nullptr) {
		localPosition = newRotation - parent->getWorldRotation();
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
glm::vec3 Transform::getWorldRotation() {
	if (parent != nullptr) {
		return parent->getWorldRotation() + localRotation;
	}
	else {
		return localRotation;
	}
}