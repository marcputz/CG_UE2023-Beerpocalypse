#include "StaticCube.h"

StaticCube::StaticCube(MyShader* shader, PxPhysics* physics) : GameObject("Static Cube", shader, physics, "cube/brick_cube/cube.obj", true) 
{
	PxMaterial* material = physics->createMaterial(0.5, 0.2, 0.8);
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