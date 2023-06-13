#include "FakeWall.h"

FakeWall::FakeWall(MyShader * shader, PxPhysics * physics) : NewGameObject("Fake Wall", shader, physics, "cube/brick_cube/cube.obj", true) 
{
	PxMaterial* material = physics->createMaterial(0.5, 0.2, 0.8);
	PxBoxGeometry geometry = PxBoxGeometry(1, 1, 1);
	PxShape* collider = physics->createShape(geometry, *material);

	setCollider(collider);
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

void FakeWall::onCollision(NewGameObject* otherObject)
{
}

void FakeWall::onHealthChange(int oldHealth, int newHealth)
{
}

void FakeWall::onActivate()
{
	setVisible(false);
	enableCollider(false);
}

void FakeWall::onDeactivate()
{
	setVisible(true);
	enableCollider(true);
}
