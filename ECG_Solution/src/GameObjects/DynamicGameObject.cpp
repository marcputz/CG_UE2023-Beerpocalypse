#include "DynamicGameObject.h"

DynamicGameObject::DynamicGameObject(std::string const& path) : GameObject(path) {

}

DynamicGameObject::~DynamicGameObject() {
}

void DynamicGameObject::update(float deltaT) {
	//std::cout << "Updating " << n << std::endl;
}
