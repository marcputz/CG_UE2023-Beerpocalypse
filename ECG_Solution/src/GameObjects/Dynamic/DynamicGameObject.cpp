#include "DynamicGameObject.h"

DynamicGameObject::DynamicGameObject(const std::string& path, MyShader& shader, PxPhysics* physics) : GameObject(path, shader) {
	PxMaterial* pxMaterial = physics->createMaterial(0.5, 0.5, 0.5);
	PxShape* pxShape = physics->createShape(PxBoxGeometry(1, 1, 1), *pxMaterial);
	actor_ = physics->createRigidDynamic(PxTransform(0, 0, 0));
	actor_->attachShape(*pxShape);
}

void DynamicGameObject::setPosition(float x, float y, float z) {
	actor_->setGlobalPose(PxTransform(x, y, z));
}

void DynamicGameObject::resetIntertia() {
	PxRigidDynamic* rigidDynamic = static_cast<PxRigidDynamic*>(actor_);
	rigidDynamic->setLinearVelocity(PxVec3(0, 0, 0));
	rigidDynamic->setAngularVelocity(PxVec3(0, 0, 0));
}