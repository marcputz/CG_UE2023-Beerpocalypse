#include "DynamicCube.h"

DynamicCube::DynamicCube(MyShader* shader, PxPhysics* physics) 
	: NewGameObject("Dynamic Cube", shader, physics, "cube/brick_cube/cube.obj", glm::vec3(0.5, 0.5, 0.5), false) {

}

void DynamicCube::onBeforeUpdate() {

}

void DynamicCube::onUpdate(float deltaTime) {

}