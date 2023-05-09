#pragma once

#include "../MyLight.h"
#include "../../MyFPSCamera.h"

class MySpotLight : public MyLight {
public:
	MySpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled,
		const glm::vec3& position, const glm::vec3& direction,
		float attenuationConstant, float attenuationLinear, float attenuationQuadratic,
		float cutOffAngle, float outerCutOffAngle, MyFPSCamera& camera);

	void setUniforms(unsigned int index);

protected:
	glm::vec3 position_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 direction_ = { 0.0f, -1.0f, 0.0f };
	float attenuationConstant_;
	float attenuationLinear_;
	float attenuationQuadratic_;
	float cutOffAngle_;
	float outerCutOffAngle_;
	MyFPSCamera* camera_;

private:

};