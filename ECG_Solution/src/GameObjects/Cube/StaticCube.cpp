#include "StaticCube.h"

StaticCube::StaticCube(MyShader* shader, PxPhysics* physics) : NewGameObject("Static Cube", shader, physics, "cube/brick_cube/cube.obj", glm::vec3(0.5, 0.5, 0.5), true) {

}

void StaticCube::onBeforeUpdate() {

}

void StaticCube::onUpdate(float deltaTime) {

}