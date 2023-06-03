#include "DynamicCube.h"

DynamicCube::DynamicCube(MyShader* shader, PxPhysics* physics) 
	: NewGameObject("Dynamic Cube", shader, physics, "cube/brick_cube/cube.obj", false) {
	
	PxMaterial* material = physics->createMaterial(0.5, 0.2, 0.8);
	PxBoxGeometry geometry = PxBoxGeometry(1,1, 1);
	PxShape* collider = physics->createShape(geometry, *material);

	setCollider(collider);
}

void DynamicCube::onHealthChange(int oldHealth, int newHealth)
{
}

void DynamicCube::onBeforeUpdate() {

}

void DynamicCube::onUpdate(float deltaTime) {

}