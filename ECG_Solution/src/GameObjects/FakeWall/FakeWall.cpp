#include "FakeWall.h"

FakeWall::FakeWall(MyShader * shader, PxPhysics * physics) : GameObject("Fake Wall", shader, physics, "cube/brick_cube/cube.obj", true) 
{
	PxMaterial* material = physics->createMaterial(PxReal(0.5f), PxReal(0.5f), PxReal(0.5f));
	PxBoxGeometry geometry = PxBoxGeometry(1, 1, 1);

	setCollider(&geometry, material);
}

void FakeWall::onBeforeUpdate()
{
}

void FakeWall::onUpdate(float deltaTime)
{
}

void FakeWall::resetSpecifics()
{
}

void FakeWall::processWindowInput(GLFWwindow* window, float deltaTime)
{
}

void FakeWall::processMouseInput(float offsetX, float offsetY)
{
}

void FakeWall::onCollision(GameObject* otherObject)
{
}

void FakeWall::onHealthChange(int oldHealth, int newHealth)
{
}

void FakeWall::onActivate()
{
	setVisible(false);
	enableCollider(false);

	PxFilterData filterData;
	filterData.word0 = CollisionLayer::LAYER_DEFAULT; // own ID
	filterData.word1 = CollisionLayer::LAYER_DEFAULT | CollisionLayer::LAYER_PLAYER | CollisionLayer::LAYER_ENEMIES;
	physicsShape_->setSimulationFilterData(filterData);
}

void FakeWall::onDeactivate()
{
	setVisible(true);
	enableCollider(true);

	PxFilterData filterData;
	physicsShape_->setSimulationFilterData(filterData);
}
