#include "MyPointLight.h"

MyPointLight::MyPointLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled, const glm::vec3& position, float attenuationConstant, float attenuationLinear, float attenuationQuadratic) : MyLight(ambient, diffuse, specular, enabled) {
	position_ = position;
	attenuationConstant_ = attenuationConstant;
	attenuationLinear_ = attenuationLinear;
	attenuationQuadratic_ = attenuationQuadratic;
}

void MyPointLight::setUniforms(unsigned int index) {
	std::string idxString = "pointLights[" + std::to_string(index) + std::string("]");

	for (MyShader* shader : shaders_) {
		shader->use();

		shader->setVec3(idxString + ".position", position_);
		shader->setVec3(idxString + ".ambient", ambient_);
		shader->setVec3(idxString + ".diffuse", diffuse_);
		shader->setVec3(idxString + ".specular", specular_);
		shader->setBool(idxString + ".enabled", enabled_);
		shader->setFloat(idxString + ".constant", attenuationConstant_);
		shader->setFloat(idxString + ".linear", attenuationLinear_);
		shader->setFloat(idxString + ".quadratic", attenuationQuadratic_);
	}
}
