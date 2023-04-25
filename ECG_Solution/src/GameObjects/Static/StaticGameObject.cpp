#include "StaticGameObject.h"

StaticGameObject::StaticGameObject(const std::string& path, MyShader& shader, PxPhysics* physics) : GameObject(path, shader) {
	PxMaterial* pxMaterial = physics->createMaterial(0.5, 0.5, 0.5);
	PxShape* pxShape = physics->createShape(PxBoxGeometry(1, 1, 1), *pxMaterial);
	actor_ = physics->createRigidStatic(PxTransform(0, 0, 0));
	actor_->attachShape(*pxShape);
}

void StaticGameObject::setPosition(float x, float y, float z) {
	actor_->setGlobalPose(PxTransform(x, y, z));
}