#pragma once

#include "../MyLight.h"
#include "../../GameObjects/Player/NewPlayer.h"
#include "../../GameObjects/Player/Camera/PlayerCamera.h"

class MySpotLight : public MyLight {
public:
	MySpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled,
		const glm::vec3& position, const glm::vec3& direction,
		float attenuationConstant, float attenuationLinear, float attenuationQuadratic,
		float cutOffAngle, float outerCutOffAngle, NewPlayer* player = nullptr);

	void setUniforms(unsigned int index);

protected:
	glm::vec3 position_ = { 0.0f, 0.0f, 0.0f };
	glm::vec3 direction_ = { 0.0f, -1.0f, 0.0f };
	float attenuationConstant_;
	float attenuationLinear_;
	float attenuationQuadratic_;
	float cutOffAngle_;
	float outerCutOffAngle_;
	NewPlayer* player_;

private:

};