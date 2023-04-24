#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class MyTransform {

public:
	void computeModelMatrix();
	void computeModelMatrix(const glm::mat4& parentGlobalModelMatrix);
	const glm::mat4& getModelMatrix();
	void setLocalPosition(const glm::vec3& newPosition);
	const glm::vec3& getLocalPosition();
	void setRotation(const glm::vec3& newRotation);
	const glm::vec3& getRotation();
	void setScale(const glm::vec3& newScale);
	const glm::vec3& getScale();
	bool isDirty();

protected:
	glm::vec3 position_ = { 0.0f, 0.0f, 0.0f };
	// euler rotation in degrees
	glm::vec3 rotation_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale_ = { 1.0f, 1.0f, 1.0f };
	glm::mat4 model_ = glm::mat4(1.0f);
	bool dirty_;

	glm::mat4 getLocalModelMatrix();

private:


};