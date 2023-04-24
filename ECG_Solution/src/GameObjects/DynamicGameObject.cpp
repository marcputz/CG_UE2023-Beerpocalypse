#include "DynamicGameObject.h"

DynamicGameObject::DynamicGameObject(std::string const& path, MyShader& shader) : GameObject(path, shader) {

}

DynamicGameObject::~DynamicGameObject() {
}

void DynamicGameObject::update(float deltaT) {
	//std::cout << "Updating " << n << std::endl;
}
