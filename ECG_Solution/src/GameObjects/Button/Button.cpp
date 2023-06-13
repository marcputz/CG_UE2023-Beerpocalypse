#include "Button.h"

Button::Button(MyShader* shader, PxPhysics* physics) : NewGameObject("Button", shader, physics, "button/uploads_files_2691895_button.obj", true) {
	PxMaterial* material = physics->createMaterial(0.8, 0.8, 0.1);
	PxBoxGeometry geometry = PxBoxGeometry(1, 1, 0.4f);
	PxShape* collider = physics->createShape(geometry, *material);

	setCollider(collider);
}

void Button::interact() {
	std::printf("Button Interaction\n");
}

void Button::onBeforeUpdate()
{
}

void Button::onUpdate(float deltaTime)
{
}

void Button::resetSpecifics()
{
}

void Button::processWindowInput(GLFWwindow* window, float deltaTime)
{
}

void Button::processMouseInput(float offsetX, float offsetY)
{
}

void Button::onCollision(NewGameObject* otherObject)
{
}

void Button::onHealthChange(int oldHealth, int newHealth)
{
}
