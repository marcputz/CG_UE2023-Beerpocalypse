#include "DynamicCube.h"

DynamicCube::DynamicCube(MyShader* shader, PxPhysics* physics) 
	: GameObject("Dynamic Cube", shader, physics, "cube/wood_cube/cube.obj", false) {
	
	PxMaterial* material = physics->createMaterial(0.5, 0.2, 0.8);
	PxBoxGeometry* geometry = new PxBoxGeometry(1,1, 1);

	setCollider(geometry, material);
}

void DynamicCube::onHealthChange(int oldHealth, int newHealth)
{
	if (newHealth < oldHealth) {
		if (newHealth <= 0) {
			setVisible(false);
			enableCollider(false);
			MyAssetManager::playSound("wood_destroyed");
		}
		else {
			MyAssetManager::playSound("wood_hit");
		}
	}
}

void DynamicCube::resetSpecifics() {
	//std::cout << "Reset Dynamic Cube" << std::endl;
}

void DynamicCube::onBeforeUpdate() {

}

void DynamicCube::onUpdate(float deltaTime) {

}