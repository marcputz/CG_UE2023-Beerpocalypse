#pragma once

#include "../MyLight.h"

class MyDirectionalLight : public MyLight {

public:
	MyDirectionalLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled,
		const glm::vec3& direction);

	void setUniforms(unsigned int index);

protected:
	glm::vec3 direction_ = { 0.0f, -1.0f, 0.0f };

private:

};