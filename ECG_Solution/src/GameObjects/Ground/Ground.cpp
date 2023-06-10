#include "Ground.h"

Ground::Ground(MyShader* shader, PxPhysics* physics) : NewGameObject("Ground", shader, physics, "ground/cube.obj", true) {

	PxMaterial* material = physics->createMaterial(0.8, 0.5, 0.2);
	PxBoxGeometry geometry = PxBoxGeometry(1, 1, 1);
	PxShape* collider = physics->createShape(geometry, *material);
	setCollider(collider);

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