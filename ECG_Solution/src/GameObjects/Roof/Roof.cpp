#include "Roof.h"

Roof::Roof(MyShader* shader, PxPhysics* physics) : GameObject("Roof", shader, physics, "roof/cube.obj", true) {

	PxMaterial* material = physics->createMaterial(PxReal(0.8f), PxReal(0.5f), PxReal(0.2f));
	PxBoxGeometry geometry = PxBoxGeometry(1, 1, 1);

	setCollider(&geometry, material);
}

void Roof::onHealthChange(int oldHealth, int newHealth) {
}

void Roof::resetSpecifics() {
	//std::cout << "Reset Ground" << std::endl;

}

void Roof::onBeforeUpdate() {

}

void Roof::onUpdate(float deltaTime) {

}