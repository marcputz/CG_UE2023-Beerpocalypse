#include "Ground.h"

Ground::Ground(MyShader* shader, PxPhysics* physics) : NewGameObject("Ground", shader, physics, "ground/cube.obj", glm::vec3(0.5, 0.5, 0.5), true) {

}

void Ground::onBeforeUpdate() {

}

void Ground::onUpdate(float deltaTime) {

}