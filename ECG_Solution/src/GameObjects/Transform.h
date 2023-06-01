#pragma once
#include <sstream>
#include <stdio.h>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <math.h>

class Transform {
private:
	glm::vec3 localPosition;
	glm::quat localRotation;
	glm::vec3 scale;

	Transform* parent = nullptr;

public:
	Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

	void setParent(Transform* parentTransform);

	void setLocalPosition(glm::vec3 newPosition);
	void setLocalRotation(glm::quat newRotation);
	void setScale(glm::vec3 newScale);
	glm::vec3 getLocalPosition();
	glm::quat getLocalRotation();
	glm::vec3 getScale();

	void setWorldPosition(glm::vec3 newPosition);
	void setWorldRotation(glm::quat newRotation);
	glm::vec3 getWorldPosition();
	glm::quat getWorldRotation();

	glm::vec3 getForwardVector();
};