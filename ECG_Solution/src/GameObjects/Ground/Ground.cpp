#include "Ground.h"

Ground::Ground(MyShader* shader, PxPhysics* physics) : GameObject("Ground", shader, physics, "ground/cube.obj", true) {

	PxMaterial* material = physics->createMaterial(PxReal(0.8f), PxReal(0.5f), PxReal(0.2f));
	PxBoxGeometry geometry = PxBoxGeometry(1, 1, 1);

	setCollider(&geometry, material);
}

void Ground::onHealthChange(int oldHealth, int newHealth)
{
}

void Ground::resetSpecifics() {
	//std::cout << "Reset Ground" << std::endl;

}

void Ground::onBeforeUpdate() {

}

void Ground::onUpdate(float deltaTime) {

}