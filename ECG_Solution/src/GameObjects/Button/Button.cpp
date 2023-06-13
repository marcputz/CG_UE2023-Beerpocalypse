#include "Button.h"

Button::Button(MyShader* shader, PxPhysics* physics) : NewGameObject("Button", shader, physics, "", true) {
	PxMaterial* material = physics->createMaterial(0.8, 0.8, 0.1);
	PxBoxGeometry geometry = PxBoxGeometry(1, 0.4f, 1);y
	PxShape* collider = physics->createShape(geometry, *material);

	setCollider(collider);

	redButtonModel = new MyModel("button/button_red.obj");
	greenButtonModel = new MyModel("button/button_green.obj");

	if (isActivated) {
		model_ = greenButtonModel;
	}
	else {
		model_ = redButtonModel;
	}
}

Button::~Button() {
	delete redButtonModel;
	delete greenButtonModel;
}

void Button::interact() {
	isActivated = !isActivated;

	if (isActivated) {
		model_ = greenButtonModel;
	}
	else {
		model_ = redButtonModel;
	}
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
