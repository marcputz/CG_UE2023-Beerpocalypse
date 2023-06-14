#include "StaticCube.h"

StaticCube::StaticCube(MyShader* shader, PxPhysics* physics) : GameObject("Static Cube", shader, physics, "cube/brick_cube/cube.obj", true) 
{
	PxMaterial* material = physics->createMaterial(PxReal(0.5f), PxReal(0.2f), PxReal(0.8f));
	PxBoxGeometry geometry = PxBoxGeometry(1, 1, 1);

	setCollider(&geometry, material);
}

void StaticCube::onHealthChange(int oldHealth, int newHealth)
{
}

void StaticCube::resetSpecifics() {
	//std::cout << "Reset Static Cube" << std::endl;
}

void StaticCube::onBeforeUpdate() {

}

void StaticCube::onUpdate(float deltaTime) {

}