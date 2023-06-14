#include "Roof.h"

Roof::Roof(MyShader* shader, PxPhysics* physics) : GameObject("Roof", shader, physics, "roof/cube.obj", true) {

	PxMaterial* material = physics->createMaterial(0.8, 0.5, 0.2);
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