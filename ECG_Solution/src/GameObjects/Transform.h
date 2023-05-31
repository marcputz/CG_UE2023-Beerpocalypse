#pragma once
#include <sstream>
#include <stdio.h>
#include <string>
#include <glm/glm.hpp>

class Transform {
private:
	glm::vec3 localPosition;
	glm::vec3 localRotation;
	glm::vec3 scale;

	Transform* parent = nullptr;

public:
	Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	void setParent(Transform* parentTransform);

	void setLocalPosition(glm::vec3 newPosition);
	void setLocalRotation(glm::vec3 newRotation);
	void setScale(glm::vec3 newScale);
	glm::vec3 getLocalPosition();
	glm::vec3 getLocalRotation();
	glm::vec3 getScale();

	void setWorldPosition(glm::vec3 newPosition);
	void setWorldRotation(glm::vec3 newRotation);
	glm::vec3 getWorldPosition();
	glm::vec3 getWorldRotation();
};