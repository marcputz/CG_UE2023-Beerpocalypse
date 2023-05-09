#include "MyLight.h"

MyLight::MyLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, bool enabled) {
	ambient_ = ambient;
	diffuse_ = diffuse;
	specular_ = specular;
	enabled_ = enabled;
}

void MyLight::setAmbient(const glm::vec3& newAmbient) {
	ambient_ = newAmbient;
}

void MyLight::setDiffuse(const glm::vec3& newDiffuse) {
	diffuse_ = newDiffuse;
}

void MyLight::setSpecular(const glm::vec3& newSpecular) {
	specular_ = newSpecular;
}

void MyLight::setEnabled(bool newEnabled) {
	enabled_ = newEnabled;
}

void MyLight::addLightToShader(MyShader& shader) {
	shaders_.push_back(&shader);
}

const glm::vec3& MyLight::getAmbient() {
	return ambient_;
}

const glm::vec3& MyLight::getDiffuse() {
	return diffuse_;
}

const glm::vec3& MyLight::getSpecular() {
	return specular_;
}

bool MyLight::isEnabled() {
	return enabled_;
}
