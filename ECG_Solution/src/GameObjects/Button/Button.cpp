#include "Button.h"

Button::Button(MyShader* shader, PxPhysics* physics, Activatable* objectToActivate) : GameObject("Button", shader, physics, "button/button_red.obj", true) {
	linkedObject_ = objectToActivate;
	
	PxMaterial* material = physics->createMaterial(PxReal(0.8f), PxReal(0.8f), PxReal(0.1f));
	PxBoxGeometry geometry = PxBoxGeometry(1, 0.4f, 1);

	setCollider(&geometry, material);

	redButtonModel = model_;
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

	MyAssetManager::playSound("button_click");

	if (isActivated) {
		model_ = greenButtonModel;
		if (linkedObject_ != nullptr) 
			linkedObject_->onActivate();
	}
	else {
		model_ = redButtonModel;
		if (linkedObject_ != nullptr)
			linkedObject_->onDeactivate();
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
	isActivated = true;
	interact(); // turn off button 
}

void Button::processWindowInput(GLFWwindow* window, float deltaTime)
{
}

void Button::processMouseInput(float offsetX, float offsetY)
{
}

void Button::onCollision(GameObject* otherObject)
{
}

void Button::onHealthChange(int oldHealth, int newHealth)
{
}
