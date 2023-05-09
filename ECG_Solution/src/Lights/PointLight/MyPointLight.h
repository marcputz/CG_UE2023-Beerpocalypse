#pragma once

#include "../MyLight.h"

class MyPointLight : public MyLight {

public:
	MyPointLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled,
		const glm::vec3& position,
		float attenuationConstant, float attenuationLinear, float attenuationQuadratic);

	void setUniforms(unsigned int index);

protected:
	glm::vec3 position_ = { 0.0f, 0.0f, 0.0f };
	float attenuationConstant_;
	float attenuationLinear_;
	float attenuationQuadratic_;

private:

};