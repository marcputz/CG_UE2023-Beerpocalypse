#pragma once

#include <glm/glm.hpp>
#include <list>
#include <string>
#include "../MyShader.h"

class MyLight {

public:
	MyLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled);

	virtual void setUniforms(unsigned int index) = 0;

	void setAmbient(const glm::vec3& newAmbient);
	void setDiffuse(const glm::vec3& newDiffuse);
	void setSpecular(const glm::vec3& newSpecular);
	void setEnabled(bool newEnabled);
	void addLightToShader(MyShader& shader);

	const glm::vec3& getAmbient();
	const glm::vec3& getDiffuse();
	const glm::vec3& getSpecular();
	bool isEnabled();

protected:
	std::list<MyShader*> shaders_;

	glm::vec3 ambient_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 diffuse_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 specular_ = { 0.0f, 0.0f, 0.0f };

	bool enabled_;

private:


};