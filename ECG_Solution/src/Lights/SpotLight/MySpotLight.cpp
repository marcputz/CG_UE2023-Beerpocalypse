#include "MySpotLight.h"

MySpotLight::MySpotLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled, const glm::vec3& position, const glm::vec3& direction,
	float attenuationConstant, float attenuationLinear, float attenuationQuadratic, float cutOffAngle, float outerCutOffAngle, Player* player, NewGameObject* lightingSubject) : MyLight(ambient, diffuse, specular, enabled) {
	position_ = position;
	direction_ = direction;
	attenuationConstant_ = attenuationConstant;
	attenuationLinear_ = attenuationLinear;
	attenuationQuadratic_ = attenuationQuadratic;
	cutOffAngle_ = cutOffAngle;
	outerCutOffAngle_ = outerCutOffAngle;
	player_ = player;
	lightingSubject_ = lightingSubject;
}

void MySpotLight::setUniforms(unsigned int index) {
	std::string idxString = "spotLights[" + std::to_string(index) + std::string("]");

	for (MyShader* shader : shaders_) {
		shader->use();

		if (player_ != nullptr) {
			position_ = player_->getWorldPosition();
			if (player_->getActiveCameraType() == PlayerCameraType::CAMERA_FIRST_PERSON) {
				direction_ = player_->getActiveCamera()->getDirection();
			} else {
				direction_ = player_->getForwardVector();
			}
		}

		if (lightingSubject_ != nullptr) {
			enabled_ = lightingSubject_->isVisible();
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
