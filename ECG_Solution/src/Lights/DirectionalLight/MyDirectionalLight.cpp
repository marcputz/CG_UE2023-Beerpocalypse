#include "MyDirectionalLight.h"

MyDirectionalLight::MyDirectionalLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled, const glm::vec3& direction) : MyLight(ambient, diffuse, specular, enabled) {
	direction_ = direction;
}

void MyDirectionalLight::setUniforms(unsigned int index) {
	for (MyShader* shader : shaders_) {
		shader->use();

		shader->setVec3("dirLight.direction", direction_);
		shader->setVec3("dirLight.ambient", ambient_);
		shader->setVec3("dirLight.diffuse", diffuse_);
		shader->setVec3("dirLight.specular", specular_);
		shader->setBool("dirLight.enabled", enabled_);
	}
}
