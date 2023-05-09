#include "MySpotLight.h"

MySpotLight::MySpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled, const glm::vec3& position, const glm::vec3& direction, float attenuationConstant, float attenuationLinear, float attenuationQuadratic, float cutOffAngle, float outerCutOffAngle, MyFPSCamera& camera) : MyLight(ambient, diffuse, specular, enabled) {
	position_ = position;
	direction_ = direction;
	attenuationConstant_ = attenuationConstant;
	attenuationLinear_ = attenuationLinear;
	attenuationQuadratic_ = attenuationQuadratic;
	cutOffAngle_ = cutOffAngle;
	outerCutOffAngle_ = outerCutOffAngle;
	camera_ = &camera;
}

void MySpotLight::setUniforms(unsigned int index) {
	std::string idxString = "spotLights[" + std::to_string(index) + std::string("]");

	for (MyShader* shader : shaders_) {
		shader->use();

		if (camera_ != nullptr) {
			position_ = camera_->position_;
			direction_ = camera_->front_;
		}

		shader->setVec3(idxString + ".position", position_);
		shader->setVec3(idxString + ".direction", direction_);
		shader->setVec3(idxString + ".ambient", ambient_);
		shader->setVec3(idxString + ".diffuse", diffuse_);
		shader->setVec3(idxString + ".specular", specular_);
		shader->setBool(idxString + ".enabled", enabled_);
		shader->setFloat(idxString + ".constant", attenuationConstant_);
		shader->setFloat(idxString + ".linear", attenuationLinear_);
		shader->setFloat(idxString + ".quadratic", attenuationQuadratic_);
		shader->setFloat(idxString + ".cutOff", glm::cos(glm::radians(cutOffAngle_)));
		shader->setFloat(idxString + ".outerCutOff", glm::cos(glm::radians(outerCutOffAngle_)));
	}
}
